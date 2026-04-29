# FumHood App

แอปควบคุม FumHood (ESP32-S3) ผ่าน Firebase Realtime Database
สร้างด้วย Expo + React Native, ธีมสีฟ้า

## ระบบบัญชีผู้ใช้
- **Admin** → เข้าสู่ระบบด้วย **Email/Password** (เห็นได้ทุกอุปกรณ์ของทุก user)
- **User ทั่วไป** → เข้าสู่ระบบด้วย **Google Sign-In** (เห็นเฉพาะอุปกรณ์ที่ตัวเองเพิ่ม)
- **1 user เพิ่มอุปกรณ์ได้ไม่จำกัด** (เพิ่ม Device ID เข้าไปในรายการ)

## ฟีเจอร์
- ล็อกอิน 2 ช่องทาง (Admin / Google)
- รายการอุปกรณ์ของผู้ใช้ (รองรับหลายอุปกรณ์ต่อ 1 user)
- Dashboard real-time: ดูค่าเซ็นเซอร์ + สั่งเปิด/ปิดรีเลย์
- ตารางเวลา: ตั้งเวลาทำงานอัตโนมัติ (วัน/เวลา/ระยะเวลา)
- WiFi Setup: แสดง QR สำหรับเชื่อม Wi-Fi ของบอร์ด (WiFiPV)
- ลืมรหัสผ่าน (Admin)

## เริ่มใช้งาน
```powershell
cd fumhood-app
npm install
npx expo start
```

## ตั้งค่า Firebase
1. **Authentication → Sign-in method**:
   - เปิด **Email/Password**
   - เปิด **Google** → คัดลอก **Web client ID**
2. **Realtime Database** (region `asia-southeast1`):
   - URL ต้องตรงกับ `databaseURL` ใน [src/firebase.js](src/firebase.js#L17)
   - วาง rules จาก [database.rules.json](database.rules.json) ใน Database → Rules → Publish
3. **เปลี่ยน API key** ใน [src/firebase.js](src/firebase.js#L13) (ของเดิมหลุดสาธารณะ)
4. **ตั้งค่า Google OAuth** ใน [src/authConfig.js](src/authConfig.js):
   - `webClientId` = Web client ID จาก Firebase Auth → Google provider → Web SDK configuration
   - `androidClientId` / `iosClientId` = สร้างเพิ่มผ่าน Firebase Project Settings → Add app
5. **ตั้งค่า Admin** ใน [src/authConfig.js](src/authConfig.js):
   - แก้ `ADMIN_EMAILS = ['admin@fumhood.local']` เป็นอีเมลจริงของคุณ
6. **สมัคร Admin บัญชีแรก**:
   - เปิดแอป → แตะแท็บ **Admin** → **สร้างบัญชี Admin ใหม่** → ใส่อีเมลที่อยู่ใน `ADMIN_EMAILS`
   - หลังสมัคร เข้าไปใน Firebase Console → Database → manual เพิ่ม `/admins/{uid}: true`

## โครงสร้าง Database
```
/admins/{uid}: true                   ← ทำเครื่องหมาย admin (ฝั่ง server)
/users/{uid}/
   ├─ profile/    {email, displayName, photoURL, ...}
   └─ devices/{deviceId}/   {name, addedAt}
/devices/{deviceId}/
   ├─ owner: <uid>
   ├─ status/    ← ESP32 เขียน  {temp, humidity, light, gas, lastSeen}
   ├─ commands/  ← App เขียน    {mode, pump, fan, light, spray}
   └─ schedules/{pushId}/   {target, hour, minute, duration, days[], enabled}
```

## ระเบียบสิทธิ์ (Database Rules)
- **User**: อ่าน/เขียนได้เฉพาะ `users/{ตัวเอง}` และ `devices/{ที่ตัวเองเป็น owner}`
- **Admin** (`/admins/{uid}: true`): อ่าน/เขียนได้ทุกอย่าง
- **Device pairing**: user แรกที่ claim ได้เป็น owner; user อื่น claim ซ้ำไม่ได้

## ฝั่ง ESP32 (fumhood.ino) — สิ่งที่ต้องเพิ่ม
1. ติดตั้งไลบรารี:
   - `WiFiManager` (tzapu) — provisioning portal
   - `Firebase ESP Client` (mobizt)
   - `Preferences` (built-in)
2. กดปุ่ม `bt_4` (ESC) ค้าง 3 วินาที → ลบ Wi-Fi config + เข้าโหมด AP
   - SSID = `Fumhood_Setup`, password = `12345678`, Portal = `http://192.168.4.1`
3. Loop:
   - อ่าน `/devices/<MAC>/commands/*` → สั่งรีเลย์
   - เขียน `/devices/<MAC>/status/*` ทุก 5 วินาที พร้อม `lastSeen = now()`
4. Device ID = MAC ของ ESP32 (`WiFi.macAddress()`) — แสดงบนจอ LVGL ให้พิมพ์เข้าแอป

## หมายเหตุความปลอดภัย
- API key หลุดสาธารณะ → **ต้อง regenerate**
- ใช้ Firebase Realtime Database security rules ตาม `database.rules.json`
- เพิ่ม admin uid ใน `/admins/` ผ่าน Firebase Console เท่านั้น (ห้าม user เพิ่มเอง)
