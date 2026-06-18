# Implementation Plan — Web app v2 + Sensor settings page

สรุปจาก grilling session 2026-06-18. ขอบเขต: redesign web (vanilla + protocol module) + เพิ่มหน้า sensor settings + firmware S3 additions (inline style, ไม่ refactor).

## Decisions (locked)

| เรื่อง | เลือก |
|---|---|
| Transport | BLE-only, ไม่มี server |
| Stack | Vanilla JS + `protocol.js` module, ไม่มี build step |
| UI | Redesign หน้าตาใหม่หมด |
| Deploy | แทนที่ `fumhood-web/` ในที่เดิม, URL เดิม, sw CACHE → v7 |
| Sensor page ตั้ง | alert, unit(ft/ms), calibration 2 จุด, buzzer(ย้ายมารวม) |
| Gating | alert/unit/buzz = ungated; calibration = control+DIRECT+fan-on |
| Cal capture | firmware capture `windMS` เอง; web ส่ง trigger+ref |
| Raw stream | เฉพาะตอนเปิด cal (`CAL_BEGIN`/`CAL_END`) |
| FW approach | inline ใน `bleHandleCommand` ตามสไตล์เดิม |

---

## A. Firmware (S3) — `fumhood.ino` + `memory.cpp` + `memory.h`

### A1. Persist `mode_senser` ใน NVS (ตอนนี้เป็น `const char*` RAM only)

**`memory.h`** — เพิ่ม declarations (หลังบรรทัด 16):
```cpp
extern int sensor_unit;            // 0 = ms, 1 = ft
void saveSensorUnit(int unit);
void loadSensorUnit();
```

**`memory.cpp`** — เพิ่ม (ตามแบบ `saveBuzzMode`/`loadBuzzMode`):
```cpp
int sensor_unit = 0;   // 0=ms (default) 1=ft
void saveSensorUnit(int unit) {
    if (prefs.begin("settings", false)) {
        sensor_unit = (unit == 1) ? 1 : 0;
        prefs.putInt("sensor_unit", sensor_unit);
        prefs.end();
        Serial2.printf("[System] Sensor Unit Saved: %d\n", sensor_unit);
    }
}
void loadSensorUnit() {
    if (prefs.begin("settings", true)) {
        sensor_unit = prefs.getInt("sensor_unit", 0);
        prefs.end();
        Serial2.printf("[System] Sensor Unit Loaded: %d\n", sensor_unit);
    }
}
```

**`fumhood.ino`** —
- บรรทัด 775 `const char* mode_senser = "ms";` → คงไว้เป็น derived จาก `sensor_unit` หรือเปลี่ยน logic ที่เทียบ `mode_senser == "ms"` (1062, 1082, 1117, 1417) ให้ใช้ `sensor_unit == 0`. ทางง่าย: ตั้ง `mode_senser` จาก `sensor_unit` ตอน load + ตอนรับ UNIT cmd. (on-device toggle line 1417 ต้อง `saveSensorUnit` ด้วย ให้ persist)
- `setup()` — เพิ่ม `loadSensorUnit();` ใกล้ๆ `loadBuzzMode()` (~3430-3456)

### A2. Expose sensor-ready flag (warm-up) ให้ status อ่าน

ตอนนี้ `fanOnTime` เป็น `static` ใน `readWind` (line 953). ดึงออกเป็น global:
- `readWind`: เปลี่ยน `static unsigned long fanOnTime = 0;` → global `unsigned long fanOnTime = 0;` (ประกาศใกล้ globals ~908)
- เพิ่ม helper:
```cpp
static bool sensorReady() {
    bool fanOn = objects.fan && lv_obj_has_state(objects.fan, LV_STATE_CHECKED);
    return fanOn && (millis() - fanOnTime >= 5000);   // ผ่าน warm-up
}
```

### A3. raw stream during cal (CAL_BEGIN/CAL_END)

เพิ่ม global flag + ส่ง raw notify ใน loop:
```cpp
volatile bool g_calStreaming = false;   // ส่ง raw windMS ระหว่างเปิดหน้า cal
```
- ใน `loop()` (ใกล้ timed-notify): ถ้า `g_calStreaming && bleClientConnected` ทุก ~300ms ส่ง:
```cpp
String r = "{\"raw\":" + String(windMS, 3) + ",\"san\":" + String(sensorReady()?1:0) + "}";
bleTxChar->setValue((uint8_t*)r.c_str(), r.length());
bleTxChar->notify();
```
(`windMS` = global m/s ก่อน mapFloat, line 766)

### A4. BLE commands ใหม่ — `bleHandleCommand` (เพิ่ม "ก่อน" gate `!bleControlMode` ที่ line 2827 สำหรับ ungated; cal "หลัง" gate DIRECT)

**Ungated** (วางใกล้ BUZZ ~2816, ก่อน line 2827):
```cpp
if (key == "ALERT") {                         // ตั้ง alert threshold (m/s)
    float v = val.toFloat();
    if (v >= 0 && v <= 50) { saveAlertSetting(v); alert_set = v; bleNotify(); }
    Serial.printf("[BLE] ALERT -> %.2f\n", alert_set);
    return;
}
if (key == "UNIT") {                          // "ft" | "ms"
    saveSensorUnit(val.equalsIgnoreCase("ft") ? 1 : 0);
    mode_senser = (sensor_unit == 1) ? "ft" : "ms";
    bleNotify();
    Serial.printf("[BLE] UNIT -> %s\n", mode_senser);
    return;
}
if (key == "CAL_BEGIN") { g_calStreaming = true;  Serial.println("[BLE] cal begin"); return; }
if (key == "CAL_END")   { g_calStreaming = false; Serial.println("[BLE] cal end");   return; }
```

**Gated — control+DIRECT** (วาง "หลัง" `if (!directMode)` line 2840, รวมกับ FAN/LIGHT/...):
```cpp
else if (key == "CAL_ZERO") {                 // จุดศูนย์ (ไม่มีลม) — ref=0
    bool fanOn = objects.fan && lv_obj_has_state(objects.fan, LV_STATE_CHECKED);
    if (fanOn && (millis() - fanOnTime >= 5000)) {
        saveLow(windMS, 0.00);                // capture windMS ปัจจุบันเป็น rLow
        Serial.printf("[BLE] CAL_ZERO raw=%.3f\n", windMS);
    } else Serial.println("[BLE] CAL_ZERO rejected (fan/warm-up)");
}
else if (key == "CAL_HIGH") {                 // จุดสูง — ref = ค่าลมจริงที่กรอก
    bool fanOn = objects.fan && lv_obj_has_state(objects.fan, LV_STATE_CHECKED);
    float ref = val.toFloat();
    if (fanOn && (millis() - fanOnTime >= 5000) && ref > 0) {
        saveHigh(windMS, ref);
        Serial.printf("[BLE] CAL_HIGH raw=%.3f ref=%.2f\n", windMS, ref);
    } else Serial.println("[BLE] CAL_HIGH rejected");
}
```

### A5. status JSON — `bleBuildStatus` (line 2870)

เพิ่ม 3 field ก่อน `}`:
```cpp
j += "\"alert\":" + String(alert_set, 2)     + ",";
j += "\"unit\":\"" + String(mode_senser) + "\",";
j += "\"san\":"  + String(sensorReady() ? 1 : 0);
```
ขนาดประเมิน: ~186B + `alert`(~16) + `unit`(~14) + `san`(~8) ≈ **224B** < 244 (MTU 247). OK.
⚠️ ใส่ comma ให้ถูก — field สุดท้ายเดิม `extbuzz` ต้องเติม `,` ต่อท้าย.

---

## B. Web — `fumhood-web/` (redesign + protocol module)

ไฟล์: `index.html`, `style.css` (ใหม่), `app.js` (rewrite), `protocol.js` (ใหม่), `sw.js` (CACHE v7), `manifest.json` (คงเดิม).

### B1. `protocol.js` — single source of protocol truth

```js
// ===== command encode =====
export const CMD = {
  ctrl:    v => `CTRL:${v?1:0}`,
  mode:    v => `MODE:${v?1:0}`,        // 1=DIRECT 0=AUTO
  relay:   (k,v) => `${k.toUpperCase()}:${v?1:0}`,  // FAN/LIGHT/PUMP/SPRAY/RESERVE
  speed:   v => `SPEED:${v?1:0}`,
  mute:    v => `MUTE:${v?1:0}`,
  buzz:    n => `BUZZ:${n}`,
  extbuzz: v => `EXTBUZZ:${v?1:0}`,
  clearAlarm: () => `CLEARALARM`,
  rtc:     (y,mo,d,h,mn) => `RTC:${y},${mo},${d},${h},${mn}`,
  schedGet: d => `SCHEDGET:${d}`,
  sched:   (d,s,v) => `SCHED:${d},${s},${v.hs},${v.ms},${v.he},${v.me},${v.fan},${v.light},${v.pump},${v.spray},${v.reserve},${v.state}`,
  alert:   n => `ALERT:${n}`,
  unit:    u => `UNIT:${u}`,            // 'ft' | 'ms'
  calBegin:() => `CAL_BEGIN`,
  calEnd:  () => `CAL_END`,
  calZero: () => `CAL_ZERO`,
  calHigh: ref => `CAL_HIGH:${ref}`,
};

// field names ของ status (รับจาก device) — ที่เดียว
export const F = ['fan','light','pump','spray','reserve','speed','mode',
                  'silen','windMs','alarm1','alarm2','alarm3','time','ctrl',
                  'buzz','extbuzz','alert','unit','san'];

// slot wire order — ที่เดียว (ตรง firmware)
export const SLOT_FIELDS = ['hs','ms','he','me','fan','light','pump','spray','reserve','state'];
export const packSlot   = (d,s,v) => CMD.sched(d,s,v);
export const unpackSlot = arr => Object.fromEntries(SLOT_FIELDS.map((f,i)=>[f,arr[i]]));

// decode notify → {type, payload}
export function decode(txt){
  let o; try { o = JSON.parse(txt); } catch { return {type:'bad', raw:txt}; }
  if (o.raw !== undefined && o.fan === undefined) return {type:'cal', payload:o};   // raw stream
  if (o.sd !== undefined && Array.isArray(o.v))    return {type:'slot', payload:o};
  return {type:'status', payload:o};
}
```

### B2. `app.js` — ชั้นใหม่

- **transport**: `connect()/disconnect()/sendCmd()` คงตรรกะ BLE เดิม แต่ `sendCmd` รับ string จาก `CMD.*`
- **onNotify**: ใช้ `decode()` → switch type: `status`→`render(vm(o))`, `slot`→`applySchedReply`, `cal`→`renderCal(o)`, `bad`→warn
- **view-model**: `vm(status)` แปลง status→ค่าที่ UI ใช้ (เช่น `ready = ctrl&&direct`) — เป็น pure function, test ได้แยกจาก DOM
- **render(vm)**: map vm→DOM (ยังผูก DOM id แต่รับ vm ไม่ใช่ global)

### B3. หน้า sensor settings (view ที่ 3)

**index.html** — เพิ่ม nav button ที่ 3 + `<div id="view-sensor" class="hidden">`:
- card **Unit**: seg-btn ms/ft (`UNIT:ms`/`UNIT:ft`)
- card **Alert threshold**: number input + ปุ่มบันทึก (`ALERT:<n>`)
- card **Buzzer** + **Ext buzzer**: ย้ายจาก dashboard มาที่นี่
- card **Calibration**:
  - live raw display (จาก cal stream `raw`)
  - แถบสถานะ: ต้อง control+DIRECT+fan-on; ถ้า `san=0` แสดง "เปิดพัดลม + รอ warm-up"
  - ปุ่ม "Set Zero (ไม่มีลม)" → `CAL_ZERO` (disabled ถ้า san=0)
  - input ref + ปุ่ม "Set High" → `CAL_HIGH:<ref>` (disabled ถ้า san=0 หรือ ref ว่าง)
- `switchView('sensor')`: ส่ง `CAL_BEGIN`; ออกจาก view → `CAL_END`

### B4. `sw.js`
- `const CACHE = 'fumhood-ble-v7';`
- เพิ่ม `protocol.js`, `style.css` ใน ASSETS

---

## C. ลำดับงาน (sequencing)

1. **Firmware ก่อน** (web ต้องพึ่ง command/field ใหม่):
   - memory.h/cpp: `sensor_unit` + save/load
   - fumhood.ino: global `fanOnTime`, `sensorReady()`, `g_calStreaming`, cal stream ใน loop, BLE cmd ALERT/UNIT/CAL_*, status fields, `loadSensorUnit()` ใน setup, on-device unit toggle persist
   - compile → flash S3 (COM ปัจจุบัน — เช็คทุกครั้งเพราะ re-enumerate)
   - ทดสอบดิบ: Serial Monitor + เว็บเดิมยังต่อได้ (status fields เพิ่ม ไม่ทำให้เก่าพัง — เก่า ignore field ใหม่)
2. **Web**:
   - `protocol.js` → unit test pack/unpack/decode (node, ไม่ต้องมี device)
   - rewrite `app.js` ใช้ protocol module
   - redesign `index.html` + `style.css` + view-sensor
   - sw v7
   - `vercel deploy --prod --yes`
3. **ทดสอบ end-to-end**: connect → ตั้ง alert/unit เห็นค่า sync; เปิด cal → live raw ขึ้น; control+DIRECT+fan → Set Zero/High จำได้; reboot S3 → ค่าคงอยู่ (NVS)

## D. ความเสี่ยง / จุดระวัง

- **comma ใน status JSON** — bug นี้แหละที่ทำ MTU เกิน; ตรวจ field สุดท้ายไม่มี trailing comma, รวมขนาดจริงจาก Serial ก่อน deploy
- **cal stream + status notify ชน** — ทั้งคู่ใช้ `bleTxChar->notify()`; ขณะ cal เปิด มี 2 stream สลับกัน → web `decode()` ต้องแยกได้ (มี `raw` vs มี `fan`). OK เพราะ field ต่างกัน
- **`mode_senser` string compare** — เปลี่ยนมาคุมด้วย `sensor_unit` int กันพลาด `==` กับ `const char*`
- **fan re-enumerate COM** — flash ทุกครั้งเช็ค port ก่อน
- **เว็บเก่า cache** — sw v7 + network-first (มีแล้ว) บังคับ client โหลดใหม่
- **calibration ทำตอน fan ON เท่านั้น** — UX ต้องบอกชัด ไม่งั้น user งง raw=0

## E. ไฟล์ที่แตะ

```
memory.h          +3 บรรทัด (decl)
memory.cpp        +~20 (saveSensorUnit/loadSensorUnit + sensor_unit)
fumhood.ino       +~50 (fanOnTime global, sensorReady, g_calStreaming, cal stream,
                        4-6 BLE cmd, 3 status field, loadSensorUnit, unit toggle persist)
fumhood-web/protocol.js   ใหม่ (~60)
fumhood-web/app.js        rewrite (~400)
fumhood-web/index.html    redesign + view-sensor
fumhood-web/style.css     redesign
fumhood-web/sw.js         CACHE v7 + ASSETS
```
