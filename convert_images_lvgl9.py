"""
Convert LVGL 8 image C files to LVGL 9 format.

LVGL 8 LV_IMG_CF_TRUE_COLOR_ALPHA format:
  Data layout: for each pixel [color_lo, color_hi, alpha8] (interleaved)
  
LVGL 9 LV_COLOR_FORMAT_RGB565A8 format:
  Data layout: [all_color_lo, all_color_hi, ...] then [all_alpha8, ...]
  (non-interleaved: all RGB565 data first, then all alpha data)
"""

import re
import os
import glob

def parse_c_array_lvgl8(content):
    """Extract the pixel data from the LV_COLOR_DEPTH==16, LV_COLOR_16_SWAP==0 block."""
    # Find array name
    array_decl_match = re.search(
        r'const\s+(?:LV_ATTRIBUTE_\w+\s+)*uint8_t\s+(\w+)\s*\[\s*\]\s*=\s*\{',
        content
    )
    if not array_decl_match:
        return None, None, None, None
    
    array_name = array_decl_match.group(1)
    array_start = array_decl_match.start()
    
    # Find the entire array up to the closing }
    # Find start of the array body
    body_start = content.index('{', array_start) + 1
    
    # Find closing }; of the array (careful of nested #if blocks)
    # The array ends with };
    # We need to find the section for LV_COLOR_DEPTH==16 && LV_COLOR_16_SWAP==0
    pattern_16_noswap = re.compile(
        r'#if\s+LV_COLOR_DEPTH\s*==\s*16\s*&&\s*LV_COLOR_16_SWAP\s*==\s*0\s*\n'
        r'(.*?)'
        r'#(?:elif|endif)',
        re.DOTALL
    )
    
    match16 = pattern_16_noswap.search(content)
    if not match16:
        # Try without LV_COLOR_16_SWAP condition
        pattern_16 = re.compile(r'#if\s+LV_COLOR_DEPTH\s*==\s*16\b(.*?)#(?:elif|endif)', re.DOTALL)
        match16 = pattern_16.search(content)
    
    if not match16:
        return None, None, None, None
    
    pixel_section = match16.group(1) if match16.lastindex >= 1 else match16.group(0)
    hex_values = re.findall(r'0x[0-9a-fA-F]{2}', pixel_section)
    data = [int(v, 16) for v in hex_values]
    
    # Find the end of the entire array declaration (the closing };)
    # Search from array_start forward
    depth = 0
    pos = body_start - 1
    array_end = None
    for i in range(body_start - 1, len(content)):
        if content[i] == '{':
            depth += 1
        elif content[i] == '}':
            depth -= 1
            if depth == 0:
                array_end = i + 1
                # skip optional semicolon
                if array_end < len(content) and content[array_end] == ';':
                    array_end += 1
                break
    
    return data, array_name, array_start, array_end

def get_image_dims(content):
    """Extract width and height from image descriptor."""
    w_match = re.search(r'\.header\.w\s*=\s*(\d+)', content)
    h_match = re.search(r'\.header\.h\s*=\s*(\d+)', content)
    if w_match and h_match:
        return int(w_match.group(1)), int(h_match.group(1))
    return None, None

def convert_interleaved_to_rgb565a8(data, w, h):
    """
    Convert LVGL 8 interleaved format to LVGL 9 non-interleaved format.
    Input: [color_lo, color_hi, alpha] per pixel
    Output: [color_lo, color_hi, ...] (w*h*2 bytes) + [alpha, ...] (w*h bytes)
    """
    num_pixels = w * h
    expected_size = num_pixels * 3
    
    if len(data) < expected_size:
        print(f"  WARNING: Expected {expected_size} bytes, got {len(data)} bytes")
        return None
    
    rgb_data = []
    alpha_data = []
    for i in range(num_pixels):
        base = i * 3
        rgb_data.append(data[base])      # color_lo
        rgb_data.append(data[base + 1])  # color_hi
        alpha_data.append(data[base + 2])  # alpha
    
    return rgb_data + alpha_data

def format_c_array(values, name, attr_prefix):
    """Format values back into C array syntax."""
    lines = [f"const {attr_prefix}uint8_t {name}[] = {{"]
    
    row_len = 16  # bytes per line
    for i in range(0, len(values), row_len):
        chunk = values[i:i+row_len]
        hex_str = ', '.join(f'0x{v:02x}' for v in chunk)
        lines.append(f"  {hex_str},")
    
    lines.append("};")
    return '\n'.join(lines)

def update_image_descriptor(content, img_name, w, h):
    """Update the lv_img_dsc_t / lv_image_dsc_t descriptor to LVGL 9 format."""
    
    # Pattern for LVGL 8 image descriptor
    old_pattern = re.compile(
        r'const\s+lv_img_dsc_t\s+' + re.escape(img_name) + r'\s*=\s*\{[^}]+\}',
        re.DOTALL
    )
    
    num_pixels = w * h
    data_size = num_pixels * 3  # RGB565 (2 bytes) + Alpha (1 byte) per pixel
    stride = w * 2  # bytes per row for RGB565 part
    
    new_descriptor = (
        f"const lv_image_dsc_t {img_name} = {{\n"
        f"  .header.magic = LV_IMAGE_HEADER_MAGIC,\n"
        f"  .header.cf = LV_COLOR_FORMAT_RGB565A8,\n"
        f"  .header.flags = 0,\n"
        f"  .header.w = {w},\n"
        f"  .header.h = {h},\n"
        f"  .header.stride = {stride},\n"
        f"  .data_size = {data_size},\n"
        f"  .data = {img_name}_map,\n"
        f"}}"
    )
    
    match = old_pattern.search(content)
    if match:
        content = content[:match.start()] + new_descriptor + content[match.end():]
    else:
        print(f"  WARNING: Could not find old descriptor for {img_name}")
    
    return content

def convert_file(filepath):
    """Convert a single image C file from LVGL 8 to LVGL 9 format."""
    print(f"\nProcessing: {os.path.basename(filepath)}")
    
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # Check if already converted
    if 'LV_IMAGE_HEADER_MAGIC' in content or 'lv_image_dsc_t' in content:
        print(f"  Already converted to LVGL 9 format, skipping.")
        return
    
    # Check if it's a LVGL 8 image file
    if 'LV_IMG_CF_TRUE_COLOR_ALPHA' not in content:
        print(f"  No LV_IMG_CF_TRUE_COLOR_ALPHA found, skipping.")
        return
    
    # Get image dimensions
    w, h = get_image_dims(content)
    if w is None or h is None:
        print(f"  ERROR: Could not extract dimensions")
        return
    print(f"  Dimensions: {w}x{h}")
    
    # Get the array variable name (from new parser)
    _, array_name_pre, _, _ = parse_c_array_lvgl8(content)
    if array_name_pre is None:
        print(f"  ERROR: Could not find array declaration")
        return
    array_name = array_name_pre
    
    # Get the attribute prefix used in the array declaration
    attr_match = re.search(
        r'const\s+((?:LV_ATTRIBUTE_\w+\s+)*)uint8_t\s+' + re.escape(array_name),
        content
    )
    attr_prefix = attr_match.group(1) if attr_match else ''
    
    # Get image variable name (e.g., "img_alram")
    img_match = re.search(r'const\s+lv_img_dsc_t\s+(\w+)\s*=', content)
    if not img_match:
        print(f"  ERROR: Could not find image descriptor name")
        return
    img_name = img_match.group(1)
    
    # Parse pixel data
    pixel_data, _, array_start, array_end = parse_c_array_lvgl8(content)
    if pixel_data is None:
        print(f"  ERROR: Could not parse pixel data array")
        return
    
    expected = w * h * 3
    print(f"  Pixel data bytes: {len(pixel_data)} (expected {expected})")
    
    # Convert data format
    converted = convert_interleaved_to_rgb565a8(pixel_data, w, h)
    if converted is None:
        print(f"  ERROR: Conversion failed")
        return
    
    # Replace old array with new converted array  
    new_array = format_c_array(converted, array_name, attr_prefix)
    content = content[:array_start] + new_array + content[array_end:]
    
    # Update image descriptor
    content = update_image_descriptor(content, img_name, w, h)
    
    # Update extern declaration if present
    content = content.replace(
        f'extern const lv_img_dsc_t {img_name};',
        f'extern const lv_image_dsc_t {img_name};'
    )
    
    # Write converted file
    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(content)
    
    print(f"  Converted successfully: {img_name} ({len(pixel_data)} -> {len(converted)} bytes, same size)")

def main():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    image_files = glob.glob(os.path.join(script_dir, 'ui_image_*.c'))
    
    print(f"Found {len(image_files)} image files to process")
    for f in sorted(image_files):
        convert_file(f)
    print("\nDone!")

if __name__ == '__main__':
    main()
