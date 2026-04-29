# บันทึกการแก้ไข Fumhood Project

## รายการการแก้ไขทั้งหมด

### 🔧 Bug Fixes (Firmware - `fumhood.ino`)

#### 1. clearAlarms — Nested SSL Client (BUG 1)
**ปัญหา:** สร้าง `WiFiClientSecure` + `HTTPClient` ใหม่ภายใน `if (code == 200)` block ขณะที่ outer GET ยังเปิดอยู่ → 2 SSL clients พร้อมกัน → heap pressure

**แก้ไข:** เพิ่ม flag `bool didClearAlarms = false` แล้วยิง PUT หลัง `http.end()` (รูปแบบเดียวกับ `didScheduleEdit`)

---

#### 2. Alarm Latching — เปิดค้างจนกว่าจะมีคำสั่งปิด
**ปัญหา:** Alarm ดับเองจาก Power Loss Detection timeout (3.5s)

**แก้ไข:**
- ลบตัวแปร `last_serial_read_time`
- ลบ `#define POWER_LOSS_TIMEOUT_MS`
- ลบ Power Loss Detection block ใน `loop()`
- Alarm latch ON จนกว่าจะได้ `Alarm_X_OFF` จาก Serial2 หรือ `clearAlarms` จาก Firebase

---

#### 3. Alarm Label Typo (BUG 7)
`"Alram_1/2/3"` → `"Alarm_1/2/3"`

---

#### 4. App Control Mode — bt_1 / bt_2 ในโหมด APP
**ปัญหา:** ขณะอยู่ในโหมด App Control ปุ่มทุกตัวถูก lock — เปลี่ยนหน้าไม่ได้, ออกจากโหมดไม่ได้

**แก้ไข `CT_mode()`:**
- ย้าย bt_1 (เปลี่ยนหน้า) ออกมาทำงาน**ก่อน** app mode lock → กดเปลี่ยนหน้าได้แม้ในโหมด APP
- เพิ่ม bt_2 ในส่วน app mode lock → กดเพื่อออกจาก APP mode → เข้าโหมด DIRECT
  - Set `appControlMode = false` ทันที (local)
  - Set `requestExitAppMode = true` → flag ให้ `readFirebaseCommands()` เขียน `false` ไป Firebase

**แก้ไข `readFirebaseCommands()`:**
- เพิ่มการเขียน `appControlMode=false` ไป Firebase ทันทีเมื่อ `requestExitAppMode = true` (ก่อน 1s polling check)

**แก้ไข `loop()`:**
- ย้าย `readFirebaseCommands()` ออกจาก block ที่เช็ค `CUR_SCREEN == objects.main` → เรียกทุก 1 วินาทีไม่ว่าจะอยู่หน้าไหน → bt_2 เขียนค่าไป Firebase ได้แม้อยู่หน้า WORK/SENSER

---

#### 5. Reserve Device Status Push
**ปัญหา:** บอร์ดส่งสถานะแค่ fan/light/pump/spray — ขาด `reserve` ใน Firebase status JSON

**แก้ไข `sendSensorData()`:** เพิ่ม `f_reserve` + `"reserve":true/false` ใน status JSON
- ส่วน applyIfChanged (รับคำสั่งจากแอพ) + Serial2 RESERVE_ON/OFF + force push ใน sync_state ครบอยู่แล้ว

---

#### 6. Silen (Mute) — สั่งงานจากแอพได้
**ปัญหา:** silen ควบคุมได้ผ่านปุ่มกายภาพ (bt_10) เท่านั้น — แอพสั่งไม่ได้

**แก้ไข `sendSensorData()`:** เพิ่ม `"silen":true/false` ใน status JSON

**แก้ไข `readFirebaseCommands()`:**
- รับคำสั่ง `getVal("silen")` ในส่วน app control
- เงื่อนไข: เปลี่ยนค่าได้**เฉพาะเมื่อ fan เปิดอยู่**
- Update: `silen`, `isMuteOpen` (กลับด้านกัน), `warring`, `objects.sound_1` state
- ส่ง Serial2 `"Mute: OPEN"` / `"Mute: CLOSE"` ไป Nano
- เรียก `requestFirebasePush()` เพื่อยืนยันสถานะกลับ

**เพิ่ม forward declarations:** `extern bool silen; extern bool isMuteOpen; extern bool warring;`

---

### 📱 App Changes (React Native)

#### 7. DashboardScreen.js — Online Status Stale (BUG 2)
**ปัญหา:** `online` เช็คจาก `Date.now() - lastUpdated` ที่ render time → ถ้า Firebase หยุดอัปเดต component ไม่ re-render → badge ยัง "Online" ตลอดไป

**แก้ไข:** เพิ่ม `useState(0)` + `setInterval(() => setTick(t => t + 1), 5000)` → force re-render ทุก 5 วินาที

---

#### 8. DashboardScreen.js — Reserve Device
เพิ่ม `{ key: 'reserve', label: 'สำรอง', icon: 'flash' }` ใน array `RELAYS` → render โทเกิลควบคุม reserve ในหน้าหลัก

---

#### 9. DashboardScreen.js — Silen (Mute) Button
เพิ่มปุ่มควบคุม Silen (ปิดเสียง) ใต้เซ็นเซอร์ความเร็วลม:
- แสดงไอคอน `volume-mute` เมื่อ silen ON, `volume-high` เมื่อ silen OFF
- กดได้เฉพาะเมื่อ `appCtrlOn && fanOn`
- เขียน `/devices/{id}/commands/silen` (boolean) ไป Firebase
- ข้อความ hint แสดงสถานะ: ถ้า fan ปิด แสดง "เปิดพัดลมก่อน จึงจะสั่งปิดเสียงได้"

---

#### 10. ScheduleScreen.js — Reserve Device
เพิ่ม `{ key: 'reserve', label: 'สำรอง' }` ใน 2 ที่:
- `DEVICES` array (สรุป badges 'R' มีอยู่แล้ว)
- Device picker ใน editor (สำหรับเลือก target ของ schedule slot)

---

## ❗ Bugs ที่ยังไม่ได้แก้ (ผู้ใช้ยังไม่ได้สั่ง)

| # | ไฟล์ | ปัญหา |
|---|------|------|
| 3 | `ScheduleScreen.js` | `if (endMin <= startMin)` block overnight schedules (เช่น 23:00→01:00) — firmware รองรับแล้ว |
| 4 | `fumhood.ino` `sync_state()` | Dead code 5 บรรทัดแรก |
| 5 | `memory.cpp` + `memory.h` | `struct ProgramData` ซ้ำ — ควรเพิ่ม `#include "memory.h"` ใน `.cpp` |
| 6 | `funtion_control.h` | `LGFX tft;` (object definition) อยู่ใน header — ควรย้ายเป็น `extern` + ดีไฟน์ใน `.cpp` |
| 8 | `fumhood.ino` `handle_mute_button()` | `while(digitalRead(bt_10) == HIGH) { delay(10); }` block main loop ขณะ long press |

---

## 🛠️ Build & Upload

```powershell
arduino-cli compile -p COM4 --fqbn "esp32:esp32:esp32s3:FlashSize=16M,PartitionScheme=app3M_fat9M_16MB,PSRAM=disabled,FlashMode=qio,UploadSpeed=921600,CDCOnBoot=cdc" --upload "."
```

ขนาดเฟิร์มแวร์ปัจจุบัน: ~2,007 KB (63% ของ 3 MB partition)

---

## 🔥 Firebase Paths Reference

| Path | Purpose |
|------|---------|
| `/devices/{id}/status` | Sensor + relay state (push 1.5s) — รวม fan/light/pump/spray/reserve/silen/alarm1-3 |
| `/devices/{id}/commands/appControlMode` | บอร์ดอ่าน 1s; รีเซ็ต false ตอน boot; bt_2 เขียน false ทันที |
| `/devices/{id}/commands/{fan,light,pump,spray,reserve,silen,directMode}` | คำสั่งจากแอพ (เฉพาะตอน appControlMode=true) |
| `/devices/{id}/commands/clearAlarms` | true → บอร์ด clear alarm + เขียน false กลับ |
| `/devices/{id}/commands/scheduleEdit` | One-shot schedule edit |
| `/devices/{id}/nvsSchedule` | 7×6 schedule (สำรองจาก NVS) |
| `/devices/{id}/appSchedules` | App scheduler (ระบบแยก) |
