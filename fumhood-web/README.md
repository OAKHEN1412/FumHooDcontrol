# FumHood — BLE Web App

Web app ควบคุมตู้ดูดควัน FumHood ผ่าน **Bluetooth (Web Bluetooth API)** — ไม่พึ่ง Firebase/WiFi.

## รองรับ
| Platform | ใช้ได้ | วิธี |
|---|---|---|
| Android | ✅ | Chrome / Edge เปิด URL ได้เลย |
| Windows/Mac/Linux | ✅ | Chrome / Edge (เครื่องมี Bluetooth) |
| iPhone/iPad | ⚠️ | เปิดผ่านแอป **Bluefy** เท่านั้น (ฟรี App Store) — Safari/Chrome บน iOS ใช้ Web Bluetooth ไม่ได้ |

**ต้องเป็น HTTPS** (secure context). `http://localhost` ตอน dev ใช้ได้.

## รัน local (dev)
```powershell
cd fumhood-web
python -m http.server 8000
# เปิด http://localhost:8000 ใน Chrome (desktop ที่มี Bluetooth)
```
> Android ทดสอบจริง: ต้อง HTTPS — ใช้ host จริง (ดูล่าง) หรือ `chrome://flags` allow insecure origin สำหรับ IP ใน LAN.

## Deploy (HTTPS ฟรี)
- **GitHub Pages**: push โฟลเดอร์นี้ → Settings → Pages → branch/folder → ได้ URL `https://<user>.github.io/...`
- **Vercel/Netlify**: ลาก-วางโฟลเดอร์ หรือ connect repo (static, ไม่มี build step)

## BLE protocol (ตรงกับ firmware `fumhood.ino`, `#define BLE_ONLY 1`)
- Service `6e400001-b5a3-f393-e0a9-e50e24dcca9e` (Nordic UART)
- RX (write)  `6e400002-...` — ส่งคำสั่ง
- TX (notify) `6e400003-...` — รับ status JSON

คำสั่ง (ASCII 1 บรรทัด): `CTRL:1/0` `FAN:1/0` `LIGHT:1/0` `PUMP:1/0` `SPRAY:1/0` `RESERVE:1/0`
`SPEED:1/0` (1=HIGH) `MODE:1/0` (1=DIRECT) `MUTE:1/0` `CLEARALARM`

ต้องส่ง `CTRL:1` ก่อน ปุ่มอื่นถึงทำงาน (เหมือน App Control Mode เดิม). `MUTE` ใช้ได้เฉพาะตอนพัดลมเปิด.

status notify:
```json
{"fan":0,"light":0,"pump":0,"spray":0,"reserve":0,"speed":0,
 "mode":1,"silen":0,"windMs":0.0,"windFt":0,"alarm1":0,"alarm2":0,"alarm3":0,"ctrl":1}
```

## ไฟล์
- `index.html` / `style.css` / `app.js` — ตัวแอป
- `manifest.json` / `sw.js` — PWA (ติดตั้งลง home screen ได้). ไอคอน `icon-192.png`/`icon-512.png` ยังไม่ใส่ (optional)
