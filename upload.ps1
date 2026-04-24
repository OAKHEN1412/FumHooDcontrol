# ======================================================
# upload.ps1 - Auto-detect ESP32-S3 and upload firmware
# ======================================================

$SKETCH   = "fumhood.ino"
$FQBN     = "esp32:esp32:esp32s3:PartitionScheme=huge_app"
$BUILD_DIR = "C:/Temp/fumhood_build"
$SKETCH_DIR = Split-Path -Parent $MyInvocation.MyCommand.Path

Set-Location $SKETCH_DIR

Write-Host "=== FumHood Uploader ===" -ForegroundColor Cyan

# --- Step 1: Find ESP32-S3 board ---
Write-Host "`n[1/3] Searching for ESP32-S3 board..." -ForegroundColor Yellow
$boards = arduino-cli board list 2>&1
$port = $boards | Where-Object { $_ -match "COM\d+" -and $_ -match "esp32" } |
        ForEach-Object { if ($_ -match "(COM\d+)") { $matches[1] } } |
        Select-Object -First 1

if (-not $port) {
    Write-Host "ERROR: No ESP32 board found. Please check USB connection." -ForegroundColor Red
    Write-Host "`nAvailable ports:" -ForegroundColor Gray
    $boards | Write-Host
    exit 1
}

Write-Host "Found board on: $port" -ForegroundColor Green

# --- Step 2: Compile ---
Write-Host "`n[2/3] Compiling $SKETCH..." -ForegroundColor Yellow
Remove-Item $BUILD_DIR -Recurse -Force -ErrorAction SilentlyContinue
$compileOut = arduino-cli compile --fqbn $FQBN --output-dir $BUILD_DIR $SKETCH 2>&1
$errors = $compileOut | Where-Object { $_ -match "^.*error:" }
if ($errors) {
    Write-Host "Compile FAILED:" -ForegroundColor Red
    $errors | ForEach-Object { Write-Host $_ -ForegroundColor Red }
    exit 1
}
$compileOut | Where-Object { $_ -match "Sketch uses" } | Write-Host -ForegroundColor Green

# --- Step 3: Upload ---
Write-Host "`n[3/3] Uploading to $port..." -ForegroundColor Yellow
$binFile = "$BUILD_DIR/$SKETCH.bin"
$uploadOut = arduino-cli upload -p $port --fqbn $FQBN --input-dir $BUILD_DIR 2>&1
$uploadOut | Select-Object -Last 5 | ForEach-Object { Write-Host $_ }

if ($LASTEXITCODE -eq 0) {
    Write-Host "`nUpload SUCCESS on $port" -ForegroundColor Green
} else {
    Write-Host "`nUpload FAILED. Check connection and try again." -ForegroundColor Red
    exit 1
}
