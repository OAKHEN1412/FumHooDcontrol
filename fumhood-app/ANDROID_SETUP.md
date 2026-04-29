# Android Setup สำหรับ Google Play Store

## ✅ ขั้นตอนที่ 1: วาง google-services.json

1. ดาวน์โหลด `google-services.json` จาก Firebase Console:
   - ไป https://console.firebase.google.com
   - เลือก Project → Project Settings
   - ไปที่ "Service Accounts"
   - ดาวน์โหลด private key JSON file

2. วางไฟล์ที่:
   ```
   fumhood-app/android/google-services.json
   ```

## ✅ ขั้นตอนที่ 2: EAS Login

```bash
cd fumhood-app
eas login
```

ป้อน username/password จาก Expo Account (หรือสร้างใหม่ที่ https://expo.dev)

## ✅ ขั้นตอนที่ 3: Build APK/AAB สำหรับ Release

### เพื่อ Preview (APK):
```bash
eas build -p android
```

### เพื่อ Google Play Store (AAB):
```bash
eas build -p android --release
```

รอ ~15-20 นาที ระบบจะให้ URL ดาวน์โหลด

## ✅ ขั้นตอนที่ 4: Google Play Console Setup

1. สร้าง Developer Account:
   - ไป https://play.google.com/console
   - Signup (ค่าใช้งาน $25 ครั้งเดียว)

2. สร้าง App ใหม่:
   - Package name: `com.fumhood.automation`
   - Title: "FumHood - ระบบควบคุมการระบายอากาศ"

3. Upload AAB ที่ได้จาก EAS

4. เพิ่ม Info:
   - Privacy Policy URL
   - Terms of Service URL
   - Content Rating
   - Target Audience

5. Submit for Review

## ✅ ขั้นตอนที่ 5: Signing & Config ใน app.json

`eas.json` ถูกสร้างไว้แล้วที่:
```
fumhood-app/eas.json
```

Config:
- Preview builds: APK (สำหรับ testing)
- Production: AAB (สำหรับ Play Store)

## 📋 Firebase google-services.json เนื้อหา (ตัวอย่าง)

```json
{
  "type": "service_account",
  "project_id": "fumhood-ac",
  "private_key_id": "xxxxx",
  "private_key": "-----BEGIN PRIVATE KEY-----\n...\n-----END PRIVATE KEY-----\n",
  "client_email": "firebase-adminsdk-xxxxx@fumhood-ac.iam.gserviceaccount.com",
  "client_id": "xxxxx",
  "auth_uri": "https://accounts.google.com/o/oauth2/auth",
  "token_uri": "https://oauth2.googleapis.com/token",
  "auth_provider_x509_cert_url": "https://www.googleapis.com/oauth2/v1/certs",
  "client_x509_cert_url": "https://www.googleapis.com/robot/v1/metadata/x509/..."
}
```

## 🚀 วิธีทดสอบหลังสร้างบิลด์

```bash
# ใช้ Expo Go (ง่ายสำหรับ development)
npm start

# สแกน QR code ด้วย Expo Go app (iOS/Android)
```

---

**ถัดไป:** ให้ user ดาวน์โหลด google-services.json และรัน:
```bash
eas login
eas build -p android --release
```
