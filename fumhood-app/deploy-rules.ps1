# Firebase Rules Deployment Script for fumhood-ac
# Usage: .\deploy-rules.ps1

Write-Host "🔥 Firebase Rules Deployment Helper" -ForegroundColor Green
Write-Host ""
Write-Host "This script will help you deploy the updated Firebase Realtime Database rules."
Write-Host ""

# Path to rules file
$rulesPath = "database.rules.json"

if (-not (Test-Path $rulesPath)) {
    Write-Host "ERROR: $rulesPath not found!" -ForegroundColor Red
    exit 1
}

# Read rules
$rulesContent = Get-Content -Path $rulesPath -Raw
Write-Host "✓ Rules file loaded ($((Get-Item $rulesPath).Length) bytes)"
Write-Host ""

# Provide instruction for Firebase Console
Write-Host "📋 FOLLOW THESE STEPS IN FIREBASE CONSOLE:" -ForegroundColor Cyan
Write-Host ""
Write-Host "1. Open: https://console.firebase.google.com"
Write-Host "2. Select project: fumhood-ac"
Write-Host "3. Go to: Realtime Database → Rules tab"
Write-Host "4. COPY the text below (everything between the dashes):"
Write-Host ""
Write-Host "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
Write-Host ""
Write-Host $rulesContent
Write-Host ""
Write-Host "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
Write-Host ""
Write-Host "5. PASTE into the Rules editor in Firebase Console"
Write-Host "6. Click PUBLISH button"
Write-Host ""
Write-Host "✅ After publishing, you can:"
Write-Host "   - Create new user accounts"
Write-Host "   - Login with email/password"
Write-Host "   - Add/pair devices to user account"
Write-Host ""
Write-Host "🧪 TEST NEW USER FLOW:"
Write-Host "   1. Sign up with: test@fumhood.local / test123"
Write-Host "   2. Login"
Write-Host "   3. Add device with ID: 9C139E or BBC9E4"
Write-Host ""
