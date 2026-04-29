# FumHood Firebase Rules Deployment & Testing Guide

## 🚀 STEP 1: Deploy Updated Rules to Firebase

### Option A: Firebase Console (Recommended - Fastest)
1. Go to https://console.firebase.google.com
2. Select **fumhood-ac** project
3. Navigate to **Realtime Database → Rules** tab
4. Copy ALL content from `database.rules.json` file
5. Paste into the Rules editor
6. Click **PUBLISH** button

**Expected Result:** Green checkmark ✓ showing rules are deployed

---

### Option B: Firebase CLI (if you have access)
```bash
firebase login
firebase use fumhood-ac
firebase deploy --only database:rules
```

---

## ✅ STEP 2: Verify Rules Are Active

After publishing, check Firebase Console:
- Go to **Realtime Database → Data** tab
- Try to create a new entry under `/test` path
- Should succeed if authenticated, fail if not

---

## 🧪 STEP 3: Test New User Registration Flow

### Test Case 1: Sign Up & Login
1. **Open FumHood mobile app** (Expo)
2. **Click "สร้างบัญชีใหม่"** (Create Account)
3. **Fill in:**
   - Display Name: `Test User`
   - Email: `test@fumhood.local`
   - Password: `test123456`
   - Confirm: `test123456`
4. **Click "สร้างบัญชี"** (Create Account)

**Expected:** ✓ Account created successfully → Redirects to Dashboard

---

### Test Case 2: Add Device
1. **From Dashboard, click "เพิ่มอุปกรณ์"** (Add Device)
2. **Enter Device ID:** `9C139E` (or `BBC9E4`)
3. **Click "เพิ่ม"** (Add)

**Expected:** ✓ Device added to your list → Shows "Connected"

---

### Test Case 3: Verify Firebase Data
1. **Go to Firebase Console**
2. **Realtime Database → Data** tab
3. **Check these paths should exist:**

```
/users/{your_uid}/
  ├─ profile/
  │   ├─ email: test@fumhood.local
  │   ├─ displayName: Test User
  │   ├─ lastLogin: [timestamp]
  │   └─ provider: email
  │
  └─ devices/
      └─ 9C139E: true

/devices/9C139E/
  ├─ owner: {your_uid}
  ├─ status: connected
  └─ ...
```

---

## 🔑 Key Changes in New Rules

### OLD (Blocked new users):
```json
"users": {
  ".read": "auth != null && root.child('admins').child(auth.uid).val() == true"
}
```
❌ Required user to be admin to read/write

### NEW (Allows all authenticated users):
```json
"users": {
  ".read": "auth != null",
  "$uid": {
    ".read":  "auth != null && auth.uid == $uid",
    ".write": "auth != null && auth.uid == $uid"
  }
}
```
✅ Any authenticated user can read/write their own data

---

## 🚨 Troubleshooting

### "ไม่พบอุปกรณ์" (Device not found)
- ✅ Rules not deployed yet → Deploy now
- ✅ Device ID format wrong → Use `9C139E` or `BBC9E4`
- ✅ Device doesn't exist in `/devices` → Ensure ESP32 is running

### "ลงชื่อเข้าใช้ไม่สำเร็จ" (Login failed)
- ✅ Check email/password spelling
- ✅ Ensure Firebase Auth is enabled
- ✅ Check network connection

### "สร้างบัญชีสำเร็จแต่ไม่มีอุปกรณ์" (Account created but no devices)
- ✅ Rules not deployed yet
- ✅ Device ID incorrect
- ✅ ESP32 not connected to WiFi

---

## 📊 Rules Structure Summary

| Path | Read | Write | Purpose |
|------|------|-------|---------|
| `/users/{uid}/` | `auth.uid == $uid` | `auth.uid == $uid` | User profile |
| `/devices/{id}/` | `auth != null` | `owner matches user` | Device data |
| `/devices/{id}/status` | `auth != null` | `true` | Real-time sync |
| `/devices/{id}/commands` | `true` | `auth != null` | Device commands |

---

## ✨ Next Steps After Rules Deployment

1. ✅ Test new user registration (above)
2. ✅ Test device pairing (above)
3. 📱 Build Android APK: `eas build -p android --release`
4. 🍎 Build iOS IPA: `eas build -p ios --release`
5. 🎯 Submit to Google Play Store & App Store

---

## 🔗 Useful Links

- Firebase Console: https://console.firebase.google.com
- fumhood-ac project: https://console.firebase.google.com/project/fumhood-ac
- Firebase Rules Docs: https://firebase.google.com/docs/database/security/rules-conditions

---

**Created:** $(Get-Date -Format "yyyy-MM-dd HH:mm:ss")
**For:** FumHood IoT System v1.0.0
**Status:** Rules deployment pending user action ⏳
