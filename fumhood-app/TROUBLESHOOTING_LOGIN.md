# 🔧 Troubleshooting: Email/Password Login Not Working

## ✅ What We Fixed
1. ✅ app.json schema error - removed `displayName`
2. ✅ Missing `expo-font` dependency - installed
3. ✅ Duplicate dependencies - resolved
4. ✅ Metro bundler - running successfully

---

## ❌ Current Issue
Login with Email/Password is not working, but Admin login was working before.

---

## 🔍 Diagnostic Checklist

### 1. Firebase Authentication - Email/Password
Go to: https://console.firebase.google.com/project/fumhood-ac/authentication/providers

**Check these boxes are ENABLED (with green checkmark):**
- [ ] Email/Password ✅ (for regular users)
- [ ] Anonymous (optional)

**How to enable Email/Password if disabled:**
1. Click on **Email/Password** provider
2. If it shows red "Disabled" - click **Enable** button
3. Toggle **Email/Password** switch ON
4. Click **Save**

---

### 2. Firebase Database Rules
Go to: https://console.firebase.google.com/project/fumhood-ac/database/rules

**Check the Rules tab shows:**
```json
"users": {
  ".read": "auth != null",
  "$uid": {
    ".read": "auth != null && auth.uid == $uid",
    ".write": "auth != null && auth.uid == $uid"
  }
}
```

**If it still shows OLD rules with "admins" requirement:**
```json
".read": "auth != null && root.child('admins').child(auth.uid).val() == true"
```

❌ **DEPLOY NEW RULES:**
1. Copy new rules from `database.rules.json`
2. Paste into Rules editor
3. Click **Publish**

---

### 3. Test Connection
Run this to verify Firebase is accessible:
```bash
node check-firebase-rules.js
```

**Expected output:**
- ✅ Rules retrieved successfully
- ✅ NEW RULES DEPLOYED
- ✅ Ready for testing

---

## 🚀 Steps to Fix

### Step 1: Enable Email/Password Auth
1. Go to Firebase Console → Authentication
2. Click "Email/Password" provider
3. Enable it and Save
4. ✅ Should show as **Enabled** (green)

### Step 2: Deploy Updated Rules
```bash
node deploy-firebase-rules.js
```

OR manually:
1. Go to Realtime Database → Rules tab
2. Copy contents of `database.rules.json`
3. Paste into Rules editor
4. Click **Publish**

### Step 3: Test Login Flow
1. Start app: `npm start`
2. Click "สร้างบัญชีใหม่" (Create Account)
3. Enter:
   - Name: `Test User`
   - Email: `test@fumhood.local`
   - Password: `test123456`
4. Click "สร้างบัญชี" (Create Account)

**Expected:** ✅ Account created → Dashboard shown

### Step 4: Test Device Pairing
1. Click "เพิ่มอุปกรณ์" (Add Device)
2. Enter Device ID: `9C139E`
3. Click "เพิ่ม" (Add)

**Expected:** ✅ Device added → Shows in device list

---

## 🐛 Common Error Messages

### "Could not find Email/Password provider"
- ❌ Email/Password auth is disabled in Firebase
- ✅ **Fix:** Enable Email/Password in Firebase Console

### "Permission denied"
- ❌ Database rules haven't been updated/deployed
- ✅ **Fix:** Deploy new rules from `database.rules.json`

### "Network Error"
- ❌ Firebase connection issue
- ✅ **Fix:** Check internet, verify Firebase project ID

### "Auth disabled for this account"
- ❌ Email/Password authentication not enabled
- ✅ **Fix:** See Step 1 above

---

## 📋 Comparison: Admin vs Regular Login

### OLD: Admin Login (Was Working)
```javascript
// Only users in /admins/{uid} could access data
".read": "auth != null && root.child('admins').child(auth.uid).val() == true"
```
✅ Admin account would work
❌ Regular users blocked

### NEW: Email/Password Login (Should Work Now)
```javascript
// Any authenticated user can access their own data
"users": {
  ".read": "auth != null",
  "$uid": {
    ".write": "auth != null && auth.uid == $uid"
  }
}
```
✅ Any user can register & login
✅ Device pairing works

---

## ✨ What's Now Fixed

| Issue | Status | Action |
|-------|--------|--------|
| app.json schema | ✅ Fixed | Removed invalid displayName |
| expo-font dependency | ✅ Fixed | Installed expo-font |
| Duplicate versions | ✅ Fixed | Deduplicated dependencies |
| Metro bundler | ✅ Fixed | Cache reset, running smoothly |
| Firebase rules | ⏳ Pending | Need to deploy (see steps above) |
| Email/Password auth | ✅ Check needed | Verify enabled in Firebase Console |

---

## 🔗 Important Links

| Resource | Link |
|----------|------|
| Firebase Console | https://console.firebase.google.com/project/fumhood-ac |
| Authentication Settings | https://console.firebase.google.com/project/fumhood-ac/authentication/providers |
| Database Rules | https://console.firebase.google.com/project/fumhood-ac/database/rules |
| Realtime Database Data | https://console.firebase.google.com/project/fumhood-ac/database/data |

---

## ❓ Still Not Working?

**Check Firebase Console logs:**
1. Go to Authentication → Users tab
2. Try creating test account
3. Check if user appears in list
4. If not, check Console Logs for errors

**Check Database logs:**
1. Go to Realtime Database → Rules tab
2. Look for any validation errors in the editor
3. Check if rules syntax is valid (green checkmark = valid)

---

**Last Updated:** 2026-04-27
