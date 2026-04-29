# บันทึกการแก้ไข Fumhood Project

---

## Session 2 — 2026-04-29 (commit d19cc37)

### 🔧 Firmware Changes

#### S2-1. ENT Long-Press: เปิดเท่านั้น → คืนค่าเป็น Toggle
**ประวัติ:**
- แก้ครั้งแรก: `checkLongPress()` สั่งเปิดเท่านั้น (`!isSystemOn` guard)
- ผู้ใช้รายงาน: "กดปิดยังไม่ปิดเลย"
- **ผลสุดท้าย:** คืนค่าเป็น toggle — กดค้าง 3 วินาทีสลับเปิด/ปิด (`isSystemOn = !isSystemOn`)

**โค้ดปัจจุบัน (`checkLongPress()`):**
```cpp
if (!actionTriggered && (millis() - pressStartTime_btn >= 3000)) {
    actionTriggered = true;
    isSystemOn = !isSystemOn;
    if (isSystemOn) wakeUpScreen();
    else            shutDownScreen();
}
```

---

#### S2-2. Boot ไม่เปิดหน้าจออัตโนมัติ
**ปัญหา:** เดิม `setup()` มี `isSystemOn = true; wakeUpScreen();` → บอร์ดเปิดหน้าจอทันทีทุกครั้งที่ reset ไม่สามารถกดปิดได้

**แก้ไข:** เปลี่ยนเป็น:
```cpp
isSystemOn = false;
shutDownScreen();
```
→ หลัง reset บอร์ดอยู่ในสถานะปิด ต้องกด ENT ค้าง 3 วินาทีเพื่อเปิด

---

#### S2-3. Fan Mode → HIGH อัตโนมัติเมื่อลมต่ำ (เงื่อนไข fanOn แทน mode_is_auto)
**ปัญหา:** เดิมใช้ gate `mode_is_auto` → ถ้าอยู่ DIRECT mode แม้จะกดพัดลม m_fan ก็ไม่เปลี่ยนเป็น HIGH

**แก้ไข (`readWind()`):** เปลี่ยน gate เป็น `fanOn_wind = lv_obj_has_state(objects.fan, LV_STATE_CHECKED)`:
- เปิดพัดลม + ลมต่ำกว่า `alert_set` → ส่ง `FAN_HIGH` + label "HIGH" + add_state speed
- เปิดพัดลม + ลมสูงกว่า threshold → ส่ง `FAN_LOW` + label "AUTO" + clear_state speed
- ทำงานทั้งโหมด AUTO และ DIRECT ตราบใดที่พัดลมเปิดอยู่

---

#### S2-4. NVS จากแอพเร็วขึ้น — ลบ `publishNvsScheduleToFirebase` หลัง scheduleEdit
**ปัญหา:** หลังบันทึก scheduleEdit จากแอพ บอร์ดโหลด NVS 7 วัน + สร้าง JSON ขนาดใหญ่ + PUT SSL กิน ~3–5 วินาที → UI ค้าง

**แก้ไข:** ลบ `publishNvsScheduleToFirebase()` ออกจาก handler scheduleEdit (แอพรู้ค่าที่ตัวเองส่งอยู่แล้ว — ไม่จำเป็นต้อง republish กลับ)

**ก่อน:**
```cpp
if (didScheduleEdit) {
    DELETE scheduleEdit flag   // timeout 4s
    publishNvsScheduleToFirebase();  // ~3-5s blocking
}
```

**หลัง:**
```cpp
if (didScheduleEdit) {
    DELETE scheduleEdit flag   // timeout 2s เท่านั้น
}
```

---

#### S2-5. HTTP Timeout ลดทุกตัว
ลด worst-case stall จาก 4–8 วินาที → 1.5 วินาที:

| ฟังก์ชัน | เดิม | ใหม่ |
|--------|------|------|
| `sendSensorData()` PUT | 4000ms | 1500ms |
| `readFirebaseCommands()` GET | 4000ms | 1500ms |
| `sendAlarmStatusImmediately()` PATCH | 4000ms | 1500ms |
| clearAlarms PUT | 4000ms | 1500ms |
| bt_2 exit PUT | 4000ms | 1500ms |
| scheduleEdit DELETE | 4000ms | 2000ms |
| `readAppSchedules()` GET | 4000ms | 2000ms |
| `publishNvsScheduleToFirebase()` PUT | 8000ms | 3000ms |
| `readFirebaseCommands()` polling interval | 2000ms→800ms | 1500ms |

---

#### S2-6. Performance — ลดงานหนักใน loop
**ปัญหา:** บอร์ดกระตุกเป็นระยะ แม้ HTTP timeout สั้นลงแล้ว

**แก้ไข 3 จุด:**

**(a) `led_add_state()` throttle 50ms**
- เดิม: ถูกเรียกทุก loop iteration (~1kHz) → `digitalRead` + `lv_obj_has_state` × 7 ทุก ms
- ใหม่: เรียกทุก 50ms (~20Hz) ลดโหลด 95%

**(b) `led_time` GPIO — cache state**
```cpp
static int8_t last_led_time = -1;
bool s = lv_obj_has_state(objects.mode, LV_STATE_CHECKED);
if ((int8_t)s != last_led_time) {
    digitalWrite(led_time, s ? HIGH : LOW);
    last_led_time = s;
}
```
เดิมเรียก `digitalWrite` + `lv_obj_has_state` ทุก loop

**(c) `pinMode(led1, OUTPUT)` ใน `readWind()` — เรียกครั้งเดียว**
- เดิม: `pinMode(led1, OUTPUT)` ทุก 30ms ทุกครั้งที่พัดลมเปิด
- ใหม่: เรียกครั้งเดียวด้วย static flag `led1_is_output`; cache `last_led1` เพื่อ `digitalWrite` เฉพาะตอนเปลี่ยนค่า

---

#### S2-7. App สั่ง Relay ผ่าน Nano ไม่ทำงาน
**สาเหตุหลัก:** `CT_mode()` ใน APP mode block มี `return` ก่อนที่ `state_device()` จะถูกเรียก → LVGL state เปลี่ยนจากแอพแล้ว แต่ edge-detect ใน `sync_state()` ไม่ทำงาน → ไม่มี `FAN_ON/LIGHT_ON/PUMP_ON/SPRAY_ON/RESERVE_ON` ส่งออก Serial2 ไป Nano

**แก้ไข:** เพิ่ม `state_device()` ก่อน `return` ใน APP mode block:
```cpp
// [APP_CTRL] sync LVGL state → Serial2 commands ไปที่ Nano (relay)
state_device();
return; // ล็อกปุ่มอื่นๆ ทั้งสิ้น
```

---

## รายการการแก้ไขทั้งหมด (Session 1)

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
