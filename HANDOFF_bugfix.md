# HANDOFF — FumHood firmware + web (bug-hunt → features)

อัพเดต: 2026-06-12. ครอบ: bug-hunt 10 ตัว, Nano AVR port, feature ใหม่หลายอย่าง. **ยังไม่ commit git** — review ก่อน.

## Environment / build commands
- arduino-cli (bundle กับ Arduino IDE): `C:\Users\Tech3\AppData\Local\Programs\Arduino IDE\resources\app\lib\backend\resources\arduino-cli.exe` (ไม่อยู่ใน PATH)
- **S3** (จอหลัก): `fumhood.ino`, FQBN `esp32:esp32:esp32s3:PartitionScheme=huge_app`, **COM3**. build-dir `C:/Temp/fumhood_build`
- **Nano** (co-processor relay/buzzer): `nano_avr/nano_avr.ino`, **ATmega328P AVR**, FQBN `arduino:avr:nano` (new bootloader), **COM4** (FTDI FT232). build-dir `C:/Temp/nano_build`
  - ⚠️ **upload Nano ต้องถอดสาย S3 TX → Nano RX (D0) ก่อนทุกครั้ง** (D0/D1 ใช้ร่วม program + link S3, S3 ยิง 19200 กวน bootloader sync). เสร็จแล้วต่อกลับ
  - ถ้า sync fail ลอง `arduino:avr:nano:cpu=atmega328old` (57600)
- Web: `fumhood-web/`, Vercel project `fumhood-web`, prod **https://fumhood-web.vercel.app**. deploy: `cd fumhood-web; vercel deploy --prod --yes`. sw CACHE ตอนนี้ **v4** (bump ทุก deploy ที่อยากดัน asset ใหม่)

## บอร์ด ↔ บอร์ด
S3 `Serial2` (19200 8N1, RX_PIN/TX_PIN) ↔ Nano hardware `Serial` (D0/D1) — bidirectional 1 เส้น.
- S3→Nano: FAN_ON/OFF, LIGHT_*, FAN_HIGH/LOW (relay pin4 หน่วง 3s), PUMP_*, SPRAY_*, RESERVE_*, WIND_LOW/HIGH, "Mute: OPEN/CLOSE", **BUZZ:n**, restart
- Nano→S3: Alarm_1/2/3_ON/_OFF (S3 `Read_nano()` อ่าน)

## งานที่ทำ (ลง board แล้ว ยกเว้นที่ระบุ)

### Bug-hunt 10 ตัว (multi-agent workflow + adversarial verify)
- #1 critical `memory.cpp` is_ready persist ✅
- #2 high `fumhood.ino` fan-off HARD interlock pump/spray ทุกโหมด ✅
- #3 high `fumhood.ino` bleCmdBuf String→`char[192]`+portMUX (cross-task UAF) ✅
- #4 med `fumhood.ino` FT display +else กันค่าค้าง ✅
- #6 med `fumhood.ino` SCHED+Firebase validate ranges ✅
- #7 low `fumhood.ino` RTC validate ชม./นาที/วันในเดือน ✅
- #8 low `fumhood.ino` ลด delay handleWiFiSave → 200ms ✅
- #9 med `sw.js` cache-first→network-first ✅
- #10 med `app.js`+`index.html` crossOrigin+CSP ✅
- **WON'T DO (user รับความเสี่ยง):** #5 BLE bonding, #10 SRI hash จริง, #8 TLS off-loop

### Feature/แก้เพิ่ม (firmware S3)
- **mute เงียบเมื่อ fan off**: `handle_mute_button()` เช็ค fanOn, fan ดับ กด mute ส่งแค่ "Mute: CLOSE" ✅
- **help screen freeze fix**: ESC ค้าง 3วิ→QR, กดกลับไม่ค้าง. help=modal (loop early-return กัน CT_mode nav ชน lv_scr_load), exit on release, ลบ helpScr ✅
- **BLE DIRECT gate**: actuator (FAN/PUMP/SPRAY/RESERVE/SPEED/MUTE) สั่งได้เฉพาะ DIRECT mode. AUTO=reject เลย. MODE ดึงออกมาก่อน gate ✅
- **sensor warm-up 5วิ**: เปิดพัดลม→ "Wait.." 5วิ→เริ่มอ่าน/แสดง (readWind fanOnTime) ✅
- **led1 (LED ลม) INPUT ตอนไม่อ่าน**: fan off + warm-up → led1 INPUT (ไม่ติดทั้ง HIGH/LOW), OUTPUT เฉพาะตอนอ่านจริง ✅
- **buzzer pattern config (ผ่านเว็บ)** — ดูด้านล่าง 🔄 S3 upload กำลังทำ, Nano ยังต้อง re-upload

### Nano AVR port
`sketch_dec12a.ino` เดิม = ESP32 code (esp_system.h) → compile AVR ไม่ได้. port เป็น `nano_avr/nano_avr.ino`:
- 2-UART (ESP32) → hardware Serial ตัวเดียว (AVR) คุย S3 ทั้งรับ/ส่ง
- `esp_restart()` → AVR `asm("jmp 0")`, ตัด debug spam
- pin/logic เดิมครบ. ลง Nano แล้ว ✅ (ก่อนเพิ่ม buzzer)

### Web (deploy prod แล้ว)
- DIRECT gate UI: AUTO → relay/speed/mute/buzz เฟสลง+กดไม่ได้ (`ready=ctrl&&direct`)
- ลบ APK download link + ลบไฟล์ `fumhood.apk`
- buzzer settings card
- sw network-first + CACHE v4

## Feature: buzzer pattern config (3 layer)
ตั้งเสียงแจ้งเตือน buzzer (pin7 Nano, low-wind alarm) ผ่านเว็บ จำใน NVS.
- **modes**: 0=ต่อเนื่อง 1=ช้า500ms(default) 2=เร็ว150ms 3=บี๊บคู่(on120/off120/on120/off600)
- flow: web ปุ่ม → `BUZZ:n` (BLE) → S3 `saveBuzzMode()` NVS "settings"/"buzz_mode" + `Serial2 BUZZ:n` → Nano `buzzMode` + handleAlarmBlink state machine
- S3: BUZZ handler pre-gate (ตั้งได้ไม่ต้อง control mode), status JSON `"buzz"`, load+send boot, resync on fan-on
- Nano: ✅ compiled+code พร้อม แต่ **ยังไม่ upload** (ต้องถอดสาย S3 RX)

## ค้าง/ต้องทำต่อ
1. 🔄 **upload S3** (`fumhood.ino` รวม buzzer) → COM3 (compile กำลังรัน task b0u7xlp0i)
2. **re-upload Nano** (`nano_avr.ino` รวม buzzer pattern) → COM4 — ถอดสาย S3 RX ก่อน, `arduino:avr:nano`, build-dir `C:/Temp/nano_build`
3. ทดสอบ: เว็บเลือก buzzer mode → จำได้ + Nano เปลี่ยน pattern จริง; reboot S3 → โหลด NVS ส่งให้ Nano
4. git: ยังไม่ commit เลย. `fumhood-web/` ทั้ง dir ยัง untracked
