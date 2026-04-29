# 🔥 Firebase Rules Deploy

## ✅ เปลี่ยนแปลง

ไฟล์ `database.rules.json` ถูกอัปเดตแล้ว:

```diff
- "users": {
-   ".read":  "auth != null && root.child('admins').child(auth.uid).val() == true",
+ "users": {
+   ".read":  "auth != null",

- "devices": {
-   ".read":  "auth != null && root.child('admins').child(auth.uid).val() == true",
+ "devices": {
+   ".read":  "auth != null",
```

### 📌 ผลกระทบ:
- ✅ User ใหม่สามารถ **เขียน user profile** ของตัวเอง
- ✅ User ใหม่สามารถ **อ่าน device list**
- ✅ User ใหม่สามารถ **จับคู่อุปกรณ์** ได้

---

## 🚀 วิธี Deploy (2 วิธี)

### **วิธี 1: ใช้ Firebase CLI** (ง่ายที่สุด)

```bash
# 1. Login ครั้งแรก
firebase login

# 2. Set project
firebase use fumhood-ac

# 3. Deploy rules
firebase deploy --only database:rules
```

---

### **วิธี 2: ใช้ Firebase Console** (ถ้า CLI ไม่ได้)

1. ไปที่ [console.firebase.google.com](https://console.firebase.google.com)
2. เลือก **fumhood-ac** project
3. ไปที่ **Realtime Database → Rules**
4. Copy ทั้งหมดใน [database.rules.json](./database.rules.json)
5. Paste เข้าไป
6. กด **Publish**

---

## ✅ ตรวจสอบหลัง Deploy

ลอง **สมัครบัญชีใหม่** และ **จับคู่อุปกรณ์** ใหม่ should work now

---

## 📋 Rules Breakdown

```json
"users": {
  ".read":  "auth != null",           // ✅ User ใดๆ อ่านข้อมูล user ที่มีอยู่ได้
  "$uid": {
    ".read":  "auth.uid == $uid",     // ✅ อ่าน profile ตัวเอง
    ".write": "auth.uid == $uid"      // ✅ เขียน profile ตัวเอง
  }
}

"devices": {
  ".read":  "auth != null",           // ✅ User ใดๆ อ่านได้
  "$deviceId": {
    ".read":  "auth != null",         // ✅ User ใดๆ อ่านรายละเอียด device
    ".write": "auth != null && (owner == auth.uid || ไม่มี owner)"
             // ✅ จับคู่อุปกรณ์ที่ไม่มี owner หรือเป็นเจ้าของ
  }
}
```
