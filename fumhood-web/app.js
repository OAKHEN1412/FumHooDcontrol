'use strict';

// ===== BLE UUIDs (Nordic UART Service) — ต้องตรงกับ firmware =====
const SVC_UUID = '6e400001-b5a3-f393-e0a9-e50e24dcca9e';
const RX_UUID  = '6e400002-b5a3-f393-e0a9-e50e24dcca9e'; // write  (phone -> device)
const TX_UUID  = '6e400003-b5a3-f393-e0a9-e50e24dcca9e'; // notify (device -> phone)

// ===== Relay definitions (label ไทย + emoji) — mirror DashboardScreen =====
const RELAYS = [
  { key: 'fan',     cmd: 'FAN',     label: 'พัดลม',  icon: '🌀' },
  { key: 'light',   cmd: 'LIGHT',   label: 'ไฟ',     icon: '💡' },
  { key: 'pump',    cmd: 'PUMP',    label: 'ปั๊ม',    icon: '💧' },
  { key: 'spray',   cmd: 'SPRAY',   label: 'สเปรย์',  icon: '🌫️' },
  { key: 'reserve', cmd: 'RESERVE', label: 'สำรอง',  icon: '⚡' },
];

// ===== Schedule (NVS, 7 days × 6 slots) =====
const DAY_LABELS = ['อา', 'จ', 'อ', 'พ', 'พฤ', 'ศ', 'ส'];
const SLOT_DEVS = [
  { key: 'fan', label: 'พัดลม' }, { key: 'light', label: 'ไฟ' }, { key: 'pump', label: 'ปั๊ม' },
  { key: 'spray', label: 'สเปรย์' }, { key: 'reserve', label: 'สำรอง' },
];
// sched[day][slot] = {hs,ms,he,me,fan,light,pump,spray,reserve,state}
const sched = Array.from({ length: 7 }, () =>
  Array.from({ length: 6 }, () => ({ hs: 0, ms: 0, he: 0, me: 0, fan: 0, light: 0, pump: 0, spray: 0, reserve: 0, state: 0 })));
let curDay = 0;
const dirty = new Set();   // "d-s" ที่แก้ยังไม่บันทึก

const enc = new TextEncoder();
const dec = new TextDecoder();

let device = null, rxChar = null, txChar = null;
let connected = false;
let status = {};          // ค่าล่าสุดจาก notify
let writeChain = Promise.resolve();   // serialize BLE writes

// ===== DOM helpers =====
const $ = (id) => document.getElementById(id);
function toast(msg) {
  const t = $('toast');
  t.textContent = msg; t.classList.remove('hidden');
  clearTimeout(toast._t);
  toast._t = setTimeout(() => t.classList.add('hidden'), 2200);
}

// ===== Build relay rows =====
function buildRelays() {
  const g = $('relay-group');
  g.innerHTML = '';
  for (const r of RELAYS) {
    const row = document.createElement('div');
    row.className = 'card row-toggle relay-row';
    row.id = `relay-${r.key}`;
    row.innerHTML = `
      <div class="row-main">
        <div class="row-icon">${r.icon}</div>
        <div class="row-label">${r.label}</div>
      </div>
      <label class="switch"><input type="checkbox" id="sw-${r.key}" /><span class="slider"></span></label>`;
    g.appendChild(row);
    $(`sw-${r.key}`).addEventListener('change', (e) => {
      sendCmd(`${r.cmd}:${e.target.checked ? 1 : 0}`);
    });
  }
}

// ===== Transport: BLE เท่านั้น =====
// ?id=XXXXXX (จาก QR ที่เครื่อง) → filter หา FumHood-XXXXXX ตรงตัว
let DEVICE_ID = new URLSearchParams(location.search).get('id');

function bleSupported() {
  return typeof navigator !== 'undefined' && !!navigator.bluetooth;
}

async function connect() {
  if (!bleSupported()) { $('unsupported').classList.remove('hidden'); return; }
  try {
    // filter ด้วย service UUID เสมอ (เครื่อง advertise UUID นี้แน่นอน — ชื่ออาจตกไป scan
    // response แล้วบาง host มองไม่เห็น). คง name filter ไว้เป็นตัวเลือกเสริม (OR กัน) เผื่อชื่อมา.
    const filters = DEVICE_ID
      ? [{ name: `FumHood-${DEVICE_ID}` }, { services: [SVC_UUID] }]
      : [{ namePrefix: 'FumHood-' }, { services: [SVC_UUID] }];
    device = await navigator.bluetooth.requestDevice({
      filters,
      optionalServices: [SVC_UUID],
    });
    device.addEventListener('gattserverdisconnected', onDisconnected);
    const server = await device.gatt.connect();
    const svc = await server.getPrimaryService(SVC_UUID);
    rxChar = await svc.getCharacteristic(RX_UUID);
    txChar = await svc.getCharacteristic(TX_UUID);
    await txChar.startNotifications();
    txChar.addEventListener('characteristicvaluechanged', onNotify);

    connected = true;
    $('device-name').textContent = device.name || 'FumHood';
    $('device-name').classList.remove('hidden');
    setConnUI(true);
    toast('เชื่อมต่อแล้ว');
  } catch (err) {
    console.error(err);
    if (err && err.name !== 'NotFoundError') toast('เชื่อมต่อไม่สำเร็จ: ' + err.message);
  }
}

function disconnect() {
  if (device && device.gatt.connected) device.gatt.disconnect();
}

// ---- สแกน QR ที่ตัวเครื่อง (in-app) → ดึง ?id= → เชื่อมต่อทันที ----
let qrScanner = null;
function loadScript(src) {
  return new Promise((res, rej) => {
    const s = document.createElement('script');
    s.src = src; s.onload = res; s.onerror = () => rej(new Error('โหลดตัวสแกนไม่ได้'));
    // SECURITY: third-party script on a Web-Bluetooth control page must not be
    // tamperable in transit. crossOrigin enables CORS so SRI can be enforced.
    s.crossOrigin = 'anonymous';
    // TODO(security): add Subresource Integrity. Set
    //   s.integrity = 'sha384-<hash of html5-qrcode@2.3.8/html5-qrcode.min.js>';
    // Generate the hash from the EXACT pinned file (verify the download first):
    //   curl -s https://unpkg.com/html5-qrcode@2.3.8/html5-qrcode.min.js \
    //     | openssl dgst -sha384 -binary | openssl base64 -A
    // Cannot be hard-coded here without network access to verify it — a wrong
    // hash silently blocks the scanner. Until then the CSP meta in index.html
    // restricts script origins as a defense-in-depth fallback.
    document.head.appendChild(s);
  });
}
async function scanQr() {
  try {
    if (qrScanner) { await stopScan(); return; }   // กดซ้ำ = ปิด
    if (!window.Html5Qrcode) await loadScript('https://unpkg.com/html5-qrcode@2.3.8/html5-qrcode.min.js');
    $('qr-reader').classList.remove('hidden');
    $('btn-scan').textContent = '✕ ปิดสแกน';
    qrScanner = new window.Html5Qrcode('qr-reader');
    await qrScanner.start({ facingMode: 'environment' }, { fps: 10, qrbox: 220 }, async (text) => {
      // รับทั้ง URL เต็ม (?id=XXXX) หรือ id เพียวๆ
      let id = null;
      try { id = new URL(text).searchParams.get('id'); } catch (e) {}
      if (!id && /^[0-9A-Fa-f]{6}$/.test(text.trim())) id = text.trim();
      if (id) {
        DEVICE_ID = id.toUpperCase();
        await stopScan();
        toast('พบเครื่อง ' + DEVICE_ID + ' — เลือกในรายการ');
        connect();
      }
    });
  } catch (e) { toast('เปิดกล้องไม่ได้: ' + e.message); await stopScan(); }
}
async function stopScan() {
  if (qrScanner) { try { await qrScanner.stop(); qrScanner.clear(); } catch (e) {} qrScanner = null; }
  $('qr-reader').classList.add('hidden');
  $('btn-scan').textContent = '📷 สแกน QR ที่ตัวเครื่อง';
}

function onDisconnected() {
  connected = false; rxChar = null; txChar = null;
  setConnUI(false);
  toast('ตัดการเชื่อมต่อแล้ว');
}

// ===== send / receive =====
function sendCmd(line) {
  if (!connected || !rxChar) { toast('ยังไม่ได้เชื่อมต่อ'); return; }
  const data = enc.encode(line + '\n');
  // serialize เพื่อกัน GATT operation ซ้อน
  writeChain = writeChain.then(async () => {
    try {
      if (rxChar.writeValueWithoutResponse) await rxChar.writeValueWithoutResponse(data);
      else await rxChar.writeValue(data);
    } catch (e) { console.error('write fail', line, e); }
  });
  return writeChain;
}

function onNotify(e) {
  const txt = dec.decode(e.target.value);
  let obj;
  try { obj = JSON.parse(txt); } catch (err) { console.warn('bad notify', txt); return; }
  if (obj && obj.sd !== undefined && Array.isArray(obj.v)) {
    applySchedReply(obj);   // schedule slot reply
  } else {
    status = obj; render(); // status update
  }
}

// ===== schedule: receive slot from device =====
function applySchedReply(o) {
  const [hs, ms, he, me, fan, light, pump, spray, reserve, state] = o.v;
  sched[o.sd][o.ss] = { hs, ms, he, me, fan, light, pump, spray, reserve, state };
  dirty.delete(`${o.sd}-${o.ss}`);
  if (o.sd === curDay) renderSlots();
}

// ===== render UI from status =====
function setChecked(id, val) { const el = $(id); if (el) el.checked = !!val; }

function render() {
  const ctrl = status.ctrl === 1;
  const direct = status.mode === 1;     // relay/speed/mute สั่งได้เฉพาะ DIRECT (firmware บังคับด้วย)
  const ready = ctrl && direct;

  // BLE control switch
  setChecked('sw-ctrl', ctrl);

  // gate: mode/ctrl ใช้ได้เมื่อเข้า control; ตัวสั่งงานจริงต้อง DIRECT ด้วย (กันสั่งงานตอน AUTO)
  const gate = (id, on) => { const el = $(id); if (el) el.classList.toggle('gated', !on); };
  gate('mode-card', ctrl); gate('mute-card', ready);
  gate('relay-group', ready); gate('speed-card', ready);

  // mode
  $('mode-direct').classList.toggle('active', status.mode === 1);
  $('mode-auto').classList.toggle('active', status.mode === 0);

  // relays
  for (const r of RELAYS) {
    const on = status[r.key] === 1;
    setChecked(`sw-${r.key}`, on);
    const row = $(`relay-${r.key}`); if (row) row.classList.toggle('on', on);
  }

  // speed (1=high)
  $('speed-high').classList.toggle('active', status.speed === 1);
  $('speed-low').classList.toggle('active', status.speed === 0);

  // buzzer pattern setting (0=ต่อเนื่อง 1=ช้า 2=เร็ว 3=บี๊บคู่) — ใช้ได้เมื่อเชื่อมต่อ ไม่ผูก ctrl/direct
  const buzz = (typeof status.buzz === 'number') ? status.buzz : 1;
  for (let b = 0; b <= 3; b++) { const el = $(`buzz-${b}`); if (el) el.classList.toggle('active', b === buzz); }
  gate('buzz-card', true);

  // external sounder (relay pin7) — setting, ไม่ผูก ctrl/direct
  setChecked('sw-extbuzz', status.extbuzz === 1);
  gate('extbuzz-card', true);

  // mute — ใช้ได้เฉพาะตอน fan on
  setChecked('sw-mute', status.silen === 1);
  const fanOn = status.fan === 1;
  $('sw-mute').disabled = !ctrl || !fanOn;

  // sensors
  if (typeof status.windMs === 'number') {
    $('wind-ms').textContent = status.windMs.toFixed(2);
    $('wind-ft').textContent = (status.windMs * 196.8504).toFixed(0);   // device ไม่ส่ง windFt แล้ว (กัน JSON เกิน MTU)
  }

  // device RTC time
  if (status.time) $('rtc-now').textContent = status.time;

  // alarms
  const anyAlarm = status.alarm1 || status.alarm2 || status.alarm3;
  $('alarm-banner').classList.toggle('hidden', !anyAlarm);
  [1, 2, 3].forEach((n) => {
    const on = status['alarm' + n] === 1;
    const chip = $('alarm' + n);
    chip.classList.toggle('alarm-on', on);
    chip.querySelector('b').textContent = on ? 'ALARM' : 'OK';
  });
}

function setConnUI(on) {
  $('conn-pill').textContent = on ? 'เชื่อมต่อแล้ว' : 'ไม่ได้เชื่อมต่อ';
  $('conn-pill').className = 'pill ' + (on ? 'pill-on' : 'pill-off');
  $('btn-connect').classList.toggle('hidden', on);
  $('btn-disconnect').classList.toggle('hidden', !on);
  if (!on) {
    $('device-name').classList.add('hidden');
    // reset visuals
    status = {}; render();
    ['mode-card', 'mute-card', 'relay-group', 'speed-card', 'buzz-card', 'extbuzz-card'].forEach((id) => $(id).classList.add('gated'));
  }
}

// ===== schedule UI =====
const pad2 = (n) => String(n).padStart(2, '0');

function buildDayTabs() {
  const t = $('day-tabs');
  t.innerHTML = '';
  DAY_LABELS.forEach((lbl, d) => {
    const b = document.createElement('button');
    b.className = 'day-tab' + (d === curDay ? ' active' : '');
    b.textContent = lbl;
    b.addEventListener('click', () => selectDay(d));
    t.appendChild(b);
  });
}

function selectDay(d) {
  curDay = d;
  [...$('day-tabs').children].forEach((b, i) => b.classList.toggle('active', i === d));
  renderSlots();
  if (connected) sendCmd(`SCHEDGET:${d}`);   // ดึงค่าจริงจากเครื่อง (NVS)
}

function renderSlots() {
  const list = $('slot-list');
  list.innerHTML = '';
  for (let s = 0; s < 6; s++) {
    const v = sched[curDay][s];
    const isDirty = dirty.has(`${curDay}-${s}`);
    const slot = document.createElement('div');
    slot.className = 'slot' + (v.state ? ' enabled' : '') + (isDirty ? ' dirty' : '');
    slot.innerHTML = `
      <div class="slot-head">
        <div class="slot-title">ช่วงที่ ${s + 1}</div>
        <label class="switch"><input type="checkbox" ${v.state ? 'checked' : ''} data-f="state"/><span class="slider"></span></label>
      </div>
      <div class="slot-times">
        <input type="time" value="${pad2(v.hs)}:${pad2(v.ms)}" data-f="start"/>
        <span class="arrow">→</span>
        <input type="time" value="${pad2(v.he)}:${pad2(v.me)}" data-f="end"/>
      </div>
      <div class="slot-devs">
        ${SLOT_DEVS.map((d) => `<span class="chip-dev ${v[d.key] ? 'on' : ''}" data-dev="${d.key}">${d.label}</span>`).join('')}
      </div>
      <button class="btn-save" data-save="${s}">${isDirty ? 'บันทึก *' : 'บันทึก'}</button>`;
    list.appendChild(slot);

    // wire inputs
    slot.querySelector('[data-f="state"]').addEventListener('change', (e) => { v.state = e.target.checked ? 1 : 0; markDirty(s); });
    slot.querySelector('[data-f="start"]').addEventListener('change', (e) => { const [h, m] = e.target.value.split(':'); v.hs = +h; v.ms = +m; markDirty(s); });
    slot.querySelector('[data-f="end"]').addEventListener('change', (e) => { const [h, m] = e.target.value.split(':'); v.he = +h; v.me = +m; markDirty(s); });
    slot.querySelectorAll('.chip-dev').forEach((chip) => {
      chip.addEventListener('click', () => { const k = chip.dataset.dev; v[k] = v[k] ? 0 : 1; chip.classList.toggle('on', !!v[k]); markDirty(s); });
    });
    slot.querySelector('[data-save]').addEventListener('click', () => saveSlot(s));
  }
}

function markDirty(s) {
  dirty.add(`${curDay}-${s}`);
  const slot = $('slot-list').children[s];
  if (slot) { slot.classList.add('dirty'); const b = slot.querySelector('.btn-save'); if (b) b.textContent = 'บันทึก *'; }
}

// ตั้ง RTC จากเวลามือถือ → RTC:y,mo,d,h,mn
function syncRtc() {
  if (!connected) { toast('ยังไม่ได้เชื่อมต่อ'); return; }
  const d = new Date();
  sendCmd(`RTC:${d.getFullYear()},${d.getMonth() + 1},${d.getDate()},${d.getHours()},${d.getMinutes()}`);
  toast('ตั้งเวลาเครื่องแล้ว');
}

function saveSlot(s) {
  if (!connected) { toast('ยังไม่ได้เชื่อมต่อ'); return; }
  const v = sched[curDay][s];
  const csv = [curDay, s, v.hs, v.ms, v.he, v.me, v.fan, v.light, v.pump, v.spray, v.reserve, v.state].join(',');
  sendCmd(`SCHED:${csv}`);   // → firmware เขียนลง NVS แล้ว echo กลับ
  toast(`บันทึกช่วงที่ ${s + 1} แล้ว`);
}

// ===== view switching (nav) =====
function switchView(name) {
  const isSched = name === 'schedule';
  $('view-dashboard').classList.toggle('hidden', isSched);
  $('view-schedule').classList.toggle('hidden', !isSched);
  $('nav-dashboard').classList.toggle('active', !isSched);
  $('nav-schedule').classList.toggle('active', isSched);
  if (isSched) selectDay(curDay);   // โหลดวันปัจจุบันจากเครื่อง
}

// ===== wire up controls =====
function init() {
  buildRelays();
  buildDayTabs();
  renderSlots();
  $('nav-dashboard').addEventListener('click', () => switchView('dashboard'));
  $('nav-schedule').addEventListener('click', () => switchView('schedule'));

  $('btn-connect').addEventListener('click', connect);
  $('btn-scan').addEventListener('click', scanQr);
  $('btn-disconnect').addEventListener('click', disconnect);

  $('sw-ctrl').addEventListener('change', (e) => sendCmd(`CTRL:${e.target.checked ? 1 : 0}`));
  $('sw-mute').addEventListener('change', (e) => sendCmd(`MUTE:${e.target.checked ? 1 : 0}`));

  $('mode-direct').addEventListener('click', () => sendCmd('MODE:1'));
  $('mode-auto').addEventListener('click', () => sendCmd('MODE:0'));
  $('speed-high').addEventListener('click', () => sendCmd('SPEED:1'));
  $('speed-low').addEventListener('click', () => sendCmd('SPEED:0'));
  for (let b = 0; b <= 3; b++) {
    const el = $(`buzz-${b}`); if (el) el.addEventListener('click', () => sendCmd(`BUZZ:${b}`));
  }
  $('sw-extbuzz').addEventListener('change', (e) => sendCmd(`EXTBUZZ:${e.target.checked ? 1 : 0}`));
  $('alarm-banner').addEventListener('click', () => { sendCmd('CLEARALARM'); toast('เคลียร์การแจ้งเตือน'); });
  $('btn-rtc-sync').addEventListener('click', syncRtc);

  if (!bleSupported()) $('unsupported').classList.remove('hidden');
  setConnUI(false);
  if (DEVICE_ID) {
    $('device-name').textContent = 'เครื่อง: ' + DEVICE_ID + ' — กดเชื่อมต่อ';
    $('device-name').classList.remove('hidden');
  }

  if ('serviceWorker' in navigator) {
    navigator.serviceWorker.register('sw.js').catch(() => {});
  }
}

document.addEventListener('DOMContentLoaded', init);
