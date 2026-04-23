"""
Patch LovyanGFX font files to skip implementation when real LVGL is present.
Uses __has_include(<lvgl.h>) to detect if real LVGL is being compiled together.
"""

import re
import os
import glob

LOVYAN_PATH = r"C:\Users\Tech3\OneDrive\เอกสาร\Arduino\libraries\LovyanGFX\src"

def patch_montserrat_file(filepath):
    """Change '#if LV_FONT_MONTSERRAT_N' to skip when real LVGL is present."""
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # Check if already patched
    if '__has_include' in content:
        print(f"  Already patched: {os.path.basename(filepath)}")
        return

    # Find the size number from filename
    m = re.search(r'lv_font_montserrat_(\w+)\.c$', filepath)
    if not m:
        print(f"  ERROR: Could not determine font size for {os.path.basename(filepath)}")
        return
    size = m.group(1)
    macro = f"LV_FONT_MONTSERRAT_{size.upper()}"
    
    # Change: #if LV_FONT_MONTSERRAT_N
    # To: #if LV_FONT_MONTSERRAT_N && !(defined(__has_include) && __has_include(<lvgl.h>))
    old = f"#if {macro}"
    new = f"#if {macro} && !(defined(__has_include) && __has_include(<lvgl.h>))"
    
    if old not in content:
        print(f"  WARNING: Could not find '{old}' in {os.path.basename(filepath)}")
        return
    
    content = content.replace(old, new, 1)  # Replace only first occurrence
    
    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(content)
    print(f"  Patched: {os.path.basename(filepath)}")

def patch_font_fmt_txt(filepath):
    """Wrap all function definitions in font_fmt_txt.c with real-LVGL guard."""
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()
    
    if '__has_include' in content:
        print(f"  Already patched: {os.path.basename(filepath)}")
        return
    
    # Wrap the entire file content (after includes) with guard
    # Split into includes part and implementations part
    # The file starts with #include "font.h" and "#include "font_fmt_txt.h""
    include_end = content.rfind('#include')
    # Find end of last include line
    nl_pos = content.find('\n', include_end)
    
    includes_section = content[:nl_pos + 1]
    code_section = content[nl_pos + 1:]
    
    new_content = (
        includes_section +
        '\n#if !(defined(__has_include) && __has_include(<lvgl.h>))\n' +
        code_section +
        '\n#endif /* !__has_include(<lvgl.h>) */\n'
    )
    
    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(new_content)
    print(f"  Patched: {os.path.basename(filepath)}")

def main():
    # Patch Montserrat font files
    montserrat_dir = os.path.join(LOVYAN_PATH, 'lgfx', 'Fonts', 'lvgl')
    montserrat_files = glob.glob(os.path.join(montserrat_dir, 'lv_font_montserrat_*.c'))
    
    print(f"Patching {len(montserrat_files)} Montserrat font files...")
    for f in sorted(montserrat_files):
        patch_montserrat_file(f)
    
    # Patch font_fmt_txt.c
    fmt_txt_path = os.path.join(LOVYAN_PATH, 'lgfx', 'v1', 'lv_font', 'font_fmt_txt.c')
    print(f"\nPatching font_fmt_txt.c...")
    if os.path.exists(fmt_txt_path):
        patch_font_fmt_txt(fmt_txt_path)
    else:
        print(f"  ERROR: {fmt_txt_path} not found")
    
    print("\nDone!")

if __name__ == '__main__':
    main()
