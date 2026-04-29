# Multi-User Device Sharing Setup Guide

## Overview
This document describes the multi-user device sharing feature that enables one device to be connected with multiple users simultaneously.

## Architecture Changes

### Database Structure
Changed from single-owner model to multi-owner model:

**Before:**
```
/devices/{id}/owner = "user_uid" (SINGLE USER)
```

**After:**
```
/devices/{id}/owners = {
  "uid1": true,
  "uid2": true,
  "uid3": true
}
```

### Key Files Modified

1. **database.rules.json** ✅
   - Updated read/write rules to check user exists in `/owners` array
   - Changed from `owner` field to `owners` object
   - All device operations now verify membership in owners list

2. **DeviceListScreen.js** ✅
   - Line 67: Changed from `await set(ref(db, devices/${id}/owner), user.uid);`
   - To: `await set(ref(db, devices/${id}/owners/${user.uid}), true);`
   - onRemove() function now removes from both `/devices/{id}/owners/{uid}` and `/users/{uid}/devices/{id}`

3. **DeviceSharingScreen.js** ✅ (NEW)
   - Complete new screen for managing device sharing
   - List all users with access to a device
   - Share device with new users (by email)
   - Remove user access
   - Added navigation to App.js

4. **App.js** ✅
   - Added DeviceSharingScreen to navigation stack
   - Share button visible on each device in DeviceListScreen

## Manual Firebase Rules Deployment

### Method 1: Firebase Console (Recommended)
1. Open [Firebase Console](https://console.firebase.google.com/project/fumhood-ac)
2. Navigate to: **Realtime Database → Rules**
3. Replace existing rules with content from `database.rules.json`
4. Click **Publish**

### Method 2: Firebase CLI
```bash
cd fumhood-app

# Step 1: Authenticate
firebase login

# Step 2: Deploy
firebase use fumhood-ac
firebase deploy --only database:rules
```

## Updated Firebase Rules
```json
{
  "rules": {
    "admins": {
      ".read": "auth != null",
      "$uid": {
        ".write": "auth != null && root.child('admins').child(auth.uid).val() == true"
      }
    },
    "users": {
      ".read": "auth != null",
      "$uid": {
        ".read": "auth != null && auth.uid == $uid",
        ".write": "auth != null && auth.uid == $uid"
      }
    },
    "devices": {
      ".read": "auth != null",
      "$deviceId": {
        ".read": "auth != null && root.child('devices').child($deviceId).child('owners').child(auth.uid).exists()",
        ".write": "auth != null && root.child('devices').child($deviceId).child('owners').child(auth.uid).exists()",
        "owners": {
          ".validate": "newData.hasChildren()",
          "$uid": {
            ".validate": "newData.val() === true"
          }
        },
        "status": {
          ".write": "root.child('devices').child($deviceId).child('owners').child(auth.uid).exists()",
          ".read": "auth != null && root.child('devices').child($deviceId).child('owners').child(auth.uid).exists()",
          ".indexOn": ["timestamp"]
        },
        "commands": {
          ".read": "root.child('devices').child($deviceId).child('owners').child(auth.uid).exists()",
          ".write": "root.child('devices').child($deviceId).child('owners').child(auth.uid).exists()"
        },
        "schedules": {}
      }
    }
  }
}
```

## Testing the Feature

### Test 1: Pair Device (Single User)
1. Create Account 1: `user1@fumhood.local` / `password123`
2. Log in and tap "เพิ่มอุปกรณ์"
3. Enter device ID: `9C139E` (or `BBC9E4`)
4. Device should appear in "อุปกรณ์ของฉัน"

### Test 2: Share Device (Multi-User)
1. In Account 1, tap the **people icon** on the device card
2. Tap **+** icon to open share modal
3. Enter Account 2's email
4. ✅ Device now shared (see: "ฟีเจอร์นี้อยู่ระหว่างพัฒนา" placeholder for now)

### Test 3: List Shared Users
1. Tap **people icon** to view all users with access
2. See current user marked as "(ตัวคุณ - เจ้าของ)"
3. ❌ Cannot remove yourself (safety check)
4. ✅ Can remove other users

### Test 4: Device Access After Sharing
1. Log out Account 1, log in as Account 2
2. Tap "เพิ่มอุปกรณ์" and pair with same device ID
3. Both accounts now have access
4. Each user can control the device independently

## UI Changes

### DeviceListScreen
- Each device card now has two action buttons:
  - **People Icon** 👥 = Manage sharing / view shared users
  - **Trash Icon** 🗑️ = Remove device from your list

### New Screen: DeviceSharingScreen
- Location: `/src/screens/DeviceSharingScreen.js`
- Shows all users with device access
- Add new users (email-based)
- Remove user access (can't remove yourself)

## Next Steps

1. **Deploy Firebase Rules** (choose Method 1 or 2 above)
2. **Test multi-user workflow** (follow Test 1-4)
3. **Implement email-based user lookup** (currently shows placeholder)
   - Add backend API to find UID by email
   - Or: Allow users to share via copy-paste UID
4. **Add user permissions/roles** (optional)
   - Owner vs. Member distinction
   - Read-only vs. full control

## Backend Considerations

### Email-to-UID Lookup
Currently, sharing by email shows a placeholder. To fully implement:

**Option 1: Firebase Cloud Function**
```javascript
exports.getUserIdByEmail = functions.https.onCall(async (data, context) => {
  const email = data.email;
  try {
    const user = await admin.auth().getUserByEmail(email);
    return { uid: user.uid };
  } catch (error) {
    throw new functions.https.HttpsError('not-found', 'User not found');
  }
});
```

**Option 2: Store email→UID mapping in database**
```
/emails/{email} = { uid: "..." }
```

## Rollback Instructions

If needed, revert to single-owner model:
1. Change `/devices/{id}/owners/{uid}` back to `/devices/{id}/owner`
2. Update database.rules.json with original rules
3. Redeploy Firebase rules
4. Revert DeviceListScreen.js and onRemove() function

## Troubleshooting

**Issue: "ไม่มีสิทธิ์เข้าถึงอุปกรณ์"**
- Rules not deployed yet
- Check Firebase Console → Realtime Database → Rules

**Issue: Device pair fails**
- Ensure device exists in `/devices/{id}`
- Check ESP32 is online and has synced ID

**Issue: Can see device list but can't access Dashboard**
- Update rules to reflect current user in `/owners`
- Or: Remove and re-add device

## Summary of Changes

✅ **Completed:**
- Database rules updated (single owner → multi-owner)
- DeviceListScreen device pairing logic updated
- DeviceListScreen device removal logic updated
- New DeviceSharingScreen created
- Navigation integrated
- UI buttons added for share/remove actions

⏳ **Pending:**
- Firebase rules deployment
- Email-to-UID lookup implementation
- User notification system (optional)
- Share history/audit log (optional)

🔴 **Known Limitations:**
- Email-based sharing shows placeholder
- No automatic user notifications
- No share request/approval workflow
- No device transfer (ownership) feature
