# 🔐 Firebase Configuration Checklist

## ✅ Authentication Setup
https://console.firebase.google.com/project/fumhood-ac/authentication

### Required Providers
- [ ] **Email/Password** - MUST BE ENABLED ✓
  - Go to: Sign-in method tab
  - Find: "Email/Password"
  - Status: Should show "Enabled" (green ✓)
  - If not: Click → Enable → Save

- [ ] Anonymous (optional, for testing)

### NOT NEEDED (Remove if enabled)
- ❌ Google Sign-in (was causing issues before)
- ❌ Facebook, GitHub, etc.

---

## ✅ Database Rules Setup
https://console.firebase.google.com/project/fumhood-ac/database/rules

### Current Rules Status
**File location:** `fumhood-app/database.rules.json`

**Content should be:**
```json
{
  "rules": {
    "admins": { ... },
    "users": {
      ".read": "auth != null",
      "$uid": {
        ".read": "auth != null && auth.uid == $uid",
        ".write": "auth != null && auth.uid == $uid"
      }
    },
    "devices": {
      ".read": "auth != null",
      "$deviceId": { ... }
    }
  }
}
```

**Validation:**
- [ ] Rules show green checkmark ✓
- [ ] No red errors
- [ ] "auth != null" is used (not "root.child('admins'...)")

### How to Deploy
**Option 1: Using Node script**
```bash
cd fumhood-app
node deploy-firebase-rules.js
```

**Option 2: Manual copy-paste**
1. Go to: Realtime Database → Rules tab
2. Clear all content
3. Paste from `database.rules.json`
4. Click: Publish

---

## ✅ Realtime Database Setup
https://console.firebase.google.com/project/fumhood-ac/database/data

### Should See
- [ ] Location: **asia-southeast1** ✓
- [ ] Existing data from admin login attempts
- [ ] Access test: Can read `/` (if authenticated in console)

### Test Reading Data
1. Open Rules tab
2. Try to read by selecting `.read` rules
3. Should say "Authenticated users can read"

---

## ✅ Storage Setup (Optional)
Usually auto-enabled, verify:
- [ ] Cloud Storage exists
- [ ] Rules allow app access (default: deny all)

---

## 🔍 Verification Steps

### 1. Check Authentication Provider
```
Firebase Console → Authentication → Sign-in method
Look for: Email/Password → Status: Enabled ✓
```

### 2. Check Rules Are Deployed
```
Firebase Console → Realtime Database → Rules
Look for: Green checkmark ✓ (no red errors)
```

### 3. Check Rules Content
```
Should NOT see:
  "root.child('admins').child(auth.uid).val() == true"
  
Should see:
  ".read": "auth != null"
```

### 4. Test Rules
```javascript
// This should work:
GET /users.json?auth=<token>  → Success ✓

// This should fail (no auth):
GET /users.json → Permission Denied ❌
```

---

## 🆘 Common Issues

### Issue 1: "Email/Password Not Available"
**Cause:** Not enabled in Firebase
**Fix:**
1. Authentication → Sign-in method
2. Find Email/Password
3. Click Enable → Save

### Issue 2: "Permission Denied" on Login
**Cause:** Rules still have admin requirement
**Fix:**
1. Realtime Database → Rules
2. Paste from `database.rules.json`
3. Click Publish

### Issue 3: "Invalid Configuration"
**Cause:** Wrong Firebase URL or API key
**Fix:**
1. Check: `fumhood-app/src/firebase.js`
2. Compare with Firebase Console project settings
3. projectId should be: `fumhood-ac`
4. databaseURL should be: `fumhood-ac-default-rtdb.asia-southeast1.firebasedatabase.app`

---

## 📋 Before & After Rules Comparison

### BEFORE (Admin Only - Not Working)
```json
"users": {
  ".read": "auth != null && root.child('admins').child(auth.uid).val() == true"
}
```
- Only users in `/admins/{uid}` can read
- Regular users = BLOCKED ❌

### AFTER (All Authenticated Users - Should Work)
```json
"users": {
  ".read": "auth != null",
  "$uid": {
    ".read": "auth != null && auth.uid == $uid",
    ".write": "auth != null && auth.uid == $uid"
  }
}
```
- All authenticated users can read/write their own data
- Regular users = ALLOWED ✅

---

## 🔗 Links

| Purpose | URL |
|---------|-----|
| Project Overview | https://console.firebase.google.com/project/fumhood-ac |
| Authentication | https://console.firebase.google.com/project/fumhood-ac/authentication |
| Sign-in Methods | https://console.firebase.google.com/project/fumhood-ac/authentication/providers |
| Realtime Database | https://console.firebase.google.com/project/fumhood-ac/database |
| Database Rules | https://console.firebase.google.com/project/fumhood-ac/database/rules |
| Database Data | https://console.firebase.google.com/project/fumhood-ac/database/data |

---

## ✅ Verification Script

Run this to check if rules are deployed correctly:
```bash
cd fumhood-app
node check-firebase-rules.js
```

Expected output:
```
✅ Rules retrieved successfully
✅ NEW RULES DEPLOYED!
✅ Ready for testing
```

---

**Last Verified:** 2026-04-27
**Status:** Ready for deployment ⏳
