'use strict';
// Wrap everything in an IIFE: top-level `let`/`const` at global script scope
// (e.g. `let status` vs window.status, or `const UUID`) make JSC/Safari throw
// "Can't create duplicate variable" at PARSE time — the whole script then never
// runs (worked on V8/Chrome, silently dead on iOS/Bluefy). Function scope avoids it.
(function () {
// ===== on-page fatal error reporter (iOS/Bluefy has no console) =====
function showFatal(msg) {
  try {
    let b = document.getElementById('fatal');
    if (!b) {
      b = document.createElement('div');
      b.id = 'fatal';
      b.style.cssText = 'position:fixed;left:0;right:0;top:0;z-index:9999;background:#7f1d1d;color:#fff;padding:10px 14px;font:13px/1.4 monospace;white-space:pre-wrap;pointer-events:none';
      (document.body || document.documentElement).appendChild(b);
    }
    b.textContent = 'ERR: ' + msg;
  } catch (_) {}
}
window.addEventListener('error', (e) => showFatal((e.message || 'error') + ' @ ' + (e.filename || '') + ':' + (e.lineno || '')));
window.addEventListener('unhandledrejection', (e) => showFatal('promise: ' + ((e.reason && e.reason.message) || e.reason)));

const BUILD = 'v9-5';   // visible build stamp — confirms which app.js the device actually loaded
function showInfo(msg) {
  try {
    let b = document.getElementById('diag');
    if (!b) {
      b = document.createElement('div');
      b.id = 'diag';
      b.style.cssText = 'position:fixed;right:6px;top:6px;z-index:9998;background:#1e3a8a;color:#fff;padding:4px 8px;font:11px/1.2 monospace;border-radius:6px;opacity:.85;pointer-events:none';
      (document.body || document.documentElement).appendChild(b);
    }
    b.textContent = msg;
  } catch (_) {}
}

// =============================================================================
// Protocol — single source of truth for the BLE wire protocol (inlined, no
// cross-script dependency so iOS WebKit / partial SW caches can't break it).
// =============================================================================
const Protocol = (function () {
  const UUID = {
    svc: '6e400001-b5a3-f393-e0a9-e50e24dcca9e',
    rx:  '6e400002-b5a3-f393-e0a9-e50e24dcca9e', // write  (phone -> device)
    tx:  '6e400003-b5a3-f393-e0a9-e50e24dcca9e', // notify (device -> phone)
  };
  const CMD = {
    ctrl:       v => `CTRL:${v ? 1 : 0}`,
    mode:       v => `MODE:${v ? 1 : 0}`,
    relay:      (k, v) => `${k.toUpperCase()}:${v ? 1 : 0}`,
    speed:      v => `SPEED:${v ? 1 : 0}`,
    mute:       v => `MUTE:${v ? 1 : 0}`,
    buzz:       n => `BUZZ:${n}`,
    extbuzz:    v => `EXTBUZZ:${v ? 1 : 0}`,
    clearAlarm: () => `CLEARALARM`,
    rtc:        (y, mo, d, h, mn) => `RTC:${y},${mo},${d},${h},${mn}`,
    schedGet:   d => `SCHEDGET:${d}`,
    sched:      (d, s, v) => `SCHED:${[d, s, v.hs, v.ms, v.he, v.me,
                                      v.fan, v.light, v.pump, v.spray, v.reserve, v.state].join(',')}`,
    alert:      n => `ALERT:${n}`,
    unit:       u => `UNIT:${u}`,
    calBegin:   () => `CAL_BEGIN`,
    calEnd:     () => `CAL_END`,
    calZero:    () => `CAL_ZERO`,
    calHigh:    ref => `CAL_HIGH:${ref}`,
  };
  const RELAYS = [
    { key: 'fan',     label: 'พัดลม',  icon: '🌀' },
    { key: 'light',   label: 'ไฟ',     icon: '💡' },
    { key: 'pump',    label: 'ปั๊ม',    icon: '💧' },
    { key: 'spray',   label: 'สเปรย์',  icon: '🌫️' },
    { key: 'reserve', label: 'สำรอง',  icon: '⚡' },
  ];
  const SLOT_FIELDS = ['hs', 'ms', 'he', 'me', 'fan', 'light', 'pump', 'spray', 'reserve', 'state'];
  function emptySlot() { const o = {}; for (const f of SLOT_FIELDS) o[f] = 0; return o; }
  function unpackSlot(arr) { const o = {}; SLOT_FIELDS.forEach((f, i) => o[f] = arr[i]); return o; }
  function decode(txt) {
    let o;
    try { o = JSON.parse(txt); } catch (e) { return { type: 'bad', raw: txt }; }
    if (o && o.raw !== undefined && o.fan === undefined) return { type: 'cal', payload: o };
    if (o && o.sd !== undefined && Array.isArray(o.v))    return { type: 'slot', payload: o };
    return { type: 'status', payload: o };
  }
  function statusView(s) {
    s = s || {};
    const ctrl = s.ctrl === 1, direct = s.mode === 1;
    const windMs = (typeof s.windMs === 'number') ? s.windMs : null;
    const relays = {};
    for (const r of RELAYS) relays[r.key] = s[r.key] === 1;
    return {
      ctrl, direct, ready: ctrl && direct, mode: direct ? 'direct' : 'auto',
      relays, speedHigh: s.speed === 1, fanOn: s.fan === 1, mute: s.silen === 1,
      buzz: (typeof s.buzz === 'number') ? s.buzz : 1, extbuzz: s.extbuzz === 1,
      alert: (typeof s.alert === 'number') ? s.alert : null, unit: s.unit || 'ms',
      sensorReady: s.san === 1, windMs, windFt: windMs == null ? null : windMs * 196.8504,
      time: s.time || null, alarms: [s.alarm1 === 1, s.alarm2 === 1, s.alarm3 === 1],
      anyAlarm: !!(s.alarm1 || s.alarm2 || s.alarm3),
    };
  }
  return { UUID, CMD, RELAYS, SLOT_FIELDS, emptySlot, unpackSlot, decode, statusView };
})();

const { UUID, CMD, RELAYS, unpackSlot, decode, statusView } = Protocol;

// ===== schedule model (7 days × 6 slots) =====
const DAY_LABELS = ['อา', 'จ', 'อ', 'พ', 'พฤ', 'ศ', 'ส'];
const SLOT_DEVS = [
  { key: 'fan', label: 'พัดลม' }, { key: 'light', label: 'ไฟ' }, { key: 'pump', label: 'ปั๊ม' },
  { key: 'spray', label: 'สเปรย์' }, { key: 'reserve', label: 'สำรอง' },
];
const sched = Array.from({ length: 7 }, () => Array.from({ length: 6 }, () => Protocol.emptySlot()));
let curDay = 0;
const dirty = new Set();

const enc = new TextEncoder();
const dec = new TextDecoder();

let device = null, rxChar = null, txChar = null;
let connected = false;
let status = {}, view = null;
let calRaw = null, calSan = false;
let curView = 'dashboard';
let writeChain = Promise.resolve();

const $ = (id) => document.getElementById(id);
function toast(msg) {
  const t = $('toast');
  t.textContent = msg; t.classList.remove('hidden');
  clearTimeout(toast._t);
  toast._t = setTimeout(() => t.classList.add('hidden'), 2200);
}
const setChecked = (id, v) => { const el = $(id); if (el) el.checked = !!v; };
const gate = (id, on) => { const el = $(id); if (el) el.classList.toggle('gated', !on); };

// defensive event wiring — never throws if an element is missing (e.g. stale
// shell). Collects missing IDs so we can surface exactly what didn't load.
const _missing = [];
function on(id, ev, fn) {
  const el = $(id);
  if (el) el.addEventListener(ev, fn);
  else if (_missing.indexOf(id) < 0) _missing.push(id);
}

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
    $(`sw-${r.key}`).addEventListener('change', (e) => sendCmd(CMD.relay(r.key, e.target.checked)));
  }
}

let DEVICE_ID = new URLSearchParams(location.search).get('id');
const bleSupported = () => typeof navigator !== 'undefined' && !!navigator.bluetooth;

async function connect() {
  if (!bleSupported()) { $('unsupported').classList.remove('hidden'); return; }
  try {
    const filters = DEVICE_ID
      ? [{ name: `FumHood-${DEVICE_ID}` }, { services: [UUID.svc] }]
      : [{ namePrefix: 'FumHood-' }, { services: [UUID.svc] }];
    device = await navigator.bluetooth.requestDevice({ filters, optionalServices: [UUID.svc] });
    device.addEventListener('gattserverdisconnected', onDisconnected);
    const server = await device.gatt.connect();
    const svc = await server.getPrimaryService(UUID.svc);
    rxChar = await svc.getCharacteristic(UUID.rx);
    txChar = await svc.getCharacteristic(UUID.tx);
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
function disconnect() { if (device && device.gatt.connected) device.gatt.disconnect(); }
function onDisconnected() {
  connected = false; rxChar = null; txChar = null;
  setConnUI(false);
  toast('ตัดการเชื่อมต่อแล้ว');
}

let qrScanner = null;
function loadScript(src) {
  return new Promise((res, rej) => {
    const s = document.createElement('script');
    s.src = src; s.onload = res; s.onerror = () => rej(new Error('โหลดตัวสแกนไม่ได้'));
    s.crossOrigin = 'anonymous';
    document.head.appendChild(s);
  });
}
async function scanQr() {
  try {
    if (qrScanner) { await stopScan(); return; }
    if (!window.Html5Qrcode) await loadScript('https://unpkg.com/html5-qrcode@2.3.8/html5-qrcode.min.js');
    $('qr-reader').classList.remove('hidden');
    $('btn-scan').textContent = '✕ ปิดสแกน';
    qrScanner = new window.Html5Qrcode('qr-reader');
    await qrScanner.start({ facingMode: 'environment' }, { fps: 10, qrbox: 220 }, async (text) => {
      let id = null;
      try { id = new URL(text).searchParams.get('id'); } catch (e) {}
      if (!id && /^[0-9A-Fa-f]{6}$/.test(text.trim())) id = text.trim();
      if (id) { DEVICE_ID = id.toUpperCase(); await stopScan(); toast('พบเครื่อง ' + DEVICE_ID); connect(); }
    });
  } catch (e) { toast('เปิดกล้องไม่ได้: ' + e.message); await stopScan(); }
}
async function stopScan() {
  if (qrScanner) { try { await qrScanner.stop(); qrScanner.clear(); } catch (e) {} qrScanner = null; }
  $('qr-reader').classList.add('hidden');
  $('btn-scan').textContent = '📷 สแกน QR ที่ตัวเครื่อง';
}

function sendCmd(line) {
  if (!connected || !rxChar) { toast('ยังไม่ได้เชื่อมต่อ'); return; }
  const data = enc.encode(line + '\n');
  writeChain = writeChain.then(async () => {
    try {
      if (rxChar.writeValueWithoutResponse) await rxChar.writeValueWithoutResponse(data);
      else await rxChar.writeValue(data);
    } catch (e) { console.error('write fail', line, e); }
  });
  return writeChain;
}

function onNotify(e) {
  const msg = decode(dec.decode(e.target.value));
  switch (msg.type) {
    case 'status': status = msg.payload; view = statusView(status); render(); break;
    case 'slot':   applySchedReply(msg.payload); break;
    case 'cal':    calRaw = msg.payload.raw; calSan = msg.payload.san === 1; renderCal(); break;
    default:       console.warn('bad notify', msg.raw);
  }
}

function applySchedReply(o) {
  sched[o.sd][o.ss] = unpackSlot(o.v);
  dirty.delete(`${o.sd}-${o.ss}`);
  if (o.sd === curDay) renderSlots();
}

function render() {
  const vm = view || statusView({});
  setChecked('sw-ctrl', vm.ctrl);
  gate('mode-card', vm.ctrl);
  gate('relay-group', vm.ready); gate('speed-card', vm.ready); gate('mute-card', vm.ready);
  $('mode-direct').classList.toggle('active', vm.mode === 'direct');
  $('mode-auto').classList.toggle('active', vm.mode === 'auto');
  for (const r of RELAYS) {
    const on = vm.relays[r.key];
    setChecked(`sw-${r.key}`, on);
    const row = $(`relay-${r.key}`); if (row) row.classList.toggle('on', on);
  }
  $('speed-high').classList.toggle('active', vm.speedHigh);
  $('speed-low').classList.toggle('active', !vm.speedHigh);
  setChecked('sw-mute', vm.mute);
  $('sw-mute').disabled = !vm.ctrl || !vm.fanOn;
  $('wind-ms').textContent = vm.windMs == null ? '--' : vm.windMs.toFixed(2);
  $('wind-ft').textContent = vm.windFt == null ? '--' : vm.windFt.toFixed(0);
  if (vm.time) $('rtc-now').textContent = vm.time;
  $('alarm-banner').classList.toggle('hidden', !vm.anyAlarm);
  vm.alarms.forEach((on, i) => {
    const chip = $('alarm' + (i + 1));
    chip.classList.toggle('alarm-on', on);
    chip.querySelector('b').textContent = on ? 'ALARM' : 'OK';
  });
  renderSensorSettings();
  renderCal();
}

function renderSensorSettings() {
  const vm = view || statusView({});
  $('unit-ms').classList.toggle('active', vm.unit === 'ms');
  $('unit-ft').classList.toggle('active', vm.unit === 'ft');
  const ai = $('alert-input');
  if (vm.alert != null && document.activeElement !== ai && ai.value === '') ai.value = vm.alert.toFixed(1);
  for (let b = 0; b <= 3; b++) $(`buzz-${b}`).classList.toggle('active', b === vm.buzz);
  setChecked('sw-extbuzz', vm.extbuzz);
}

function renderCal() {
  const vm = view || statusView({});
  const san = calSan || vm.sensorReady;
  const ready = vm.ready;
  $('cal-raw').textContent = calRaw == null
    ? (vm.windMs == null ? '--' : vm.windMs.toFixed(3))
    : Number(calRaw).toFixed(3);
  const canCal = ready && san;
  $('btn-cal-zero').disabled = !canCal;
  $('btn-cal-high').disabled = !canCal;
  const hint = $('cal-hint');
  if (canCal) hint.classList.add('hidden');
  else {
    hint.classList.remove('hidden');
    hint.textContent = !ready
      ? 'ต้องเข้าโหมดควบคุม + DIRECT ก่อนจึงปรับเทียบได้'
      : 'เปิดพัดลมแล้วรอ warm-up ~5 วิ ก่อนจึงปรับเทียบได้';
  }
}

function setConnUI(on) {
  $('conn-pill').innerHTML = `<span class="dot"></span>${on ? 'เชื่อมต่อแล้ว' : 'ไม่ได้เชื่อมต่อ'}`;
  $('conn-pill').className = 'pill ' + (on ? 'pill-on' : 'pill-off');
  $('btn-connect').classList.toggle('hidden', on);
  $('btn-disconnect').classList.toggle('hidden', !on);
  if (!on) {
    $('device-name').classList.add('hidden');
    status = {}; view = statusView({}); calRaw = null; calSan = false;
    render();
    ['mode-card', 'mute-card', 'relay-group', 'speed-card'].forEach((id) => $(id).classList.add('gated'));
  }
}

const pad2 = (n) => String(n).padStart(2, '0');
function buildDayTabs() {
  const t = $('day-tabs'); t.innerHTML = '';
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
  if (connected) sendCmd(CMD.schedGet(d));
}
function renderSlots() {
  const list = $('slot-list'); list.innerHTML = '';
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
function syncRtc() {
  if (!connected) { toast('ยังไม่ได้เชื่อมต่อ'); return; }
  const d = new Date();
  sendCmd(CMD.rtc(d.getFullYear(), d.getMonth() + 1, d.getDate(), d.getHours(), d.getMinutes()));
  toast('ตั้งเวลาเครื่องแล้ว');
}
function saveSlot(s) {
  if (!connected) { toast('ยังไม่ได้เชื่อมต่อ'); return; }
  sendCmd(CMD.sched(curDay, s, sched[curDay][s]));
  toast(`บันทึกช่วงที่ ${s + 1} แล้ว`);
}

function switchView(name) {
  if (curView === 'sensor' && name !== 'sensor' && connected) sendCmd(CMD.calEnd());
  curView = name;
  $('view-dashboard').classList.toggle('hidden', name !== 'dashboard');
  $('view-sensor').classList.toggle('hidden', name !== 'sensor');
  $('view-schedule').classList.toggle('hidden', name !== 'schedule');
  $('nav-dashboard').classList.toggle('active', name === 'dashboard');
  $('nav-sensor').classList.toggle('active', name === 'sensor');
  $('nav-schedule').classList.toggle('active', name === 'schedule');
  if (name === 'schedule') selectDay(curDay);
  if (name === 'sensor' && connected) { calRaw = null; sendCmd(CMD.calBegin()); }
}

function init() {
  try {
    buildRelays();
    buildDayTabs();
    renderSlots();
    on('nav-dashboard', 'click', () => switchView('dashboard'));
    on('nav-sensor', 'click', () => switchView('sensor'));
    on('nav-schedule', 'click', () => switchView('schedule'));
    on('btn-connect', 'click', connect);
    on('btn-scan', 'click', scanQr);
    on('btn-disconnect', 'click', disconnect);
    on('sw-ctrl', 'change', (e) => sendCmd(CMD.ctrl(e.target.checked)));
    on('sw-mute', 'change', (e) => sendCmd(CMD.mute(e.target.checked)));
    on('mode-direct', 'click', () => sendCmd(CMD.mode(true)));
    on('mode-auto', 'click', () => sendCmd(CMD.mode(false)));
    on('speed-high', 'click', () => sendCmd(CMD.speed(true)));
    on('speed-low', 'click', () => sendCmd(CMD.speed(false)));
    on('alarm-banner', 'click', () => { sendCmd(CMD.clearAlarm()); toast('เคลียร์การแจ้งเตือน'); });
    on('btn-rtc-sync', 'click', syncRtc);
    on('unit-ms', 'click', () => sendCmd(CMD.unit('ms')));
    on('unit-ft', 'click', () => sendCmd(CMD.unit('ft')));
    on('btn-alert-save', 'click', () => {
      const v = parseFloat($('alert-input').value);
      if (isNaN(v) || v < 0) { toast('ใส่ค่า alert ให้ถูกต้อง'); return; }
      sendCmd(CMD.alert(v)); toast(`ตั้ง alert = ${v} m/s`);
    });
    for (let b = 0; b <= 3; b++) on(`buzz-${b}`, 'click', () => sendCmd(CMD.buzz(b)));
    on('sw-extbuzz', 'change', (e) => sendCmd(CMD.extbuzz(e.target.checked)));
    on('btn-cal-zero', 'click', () => { sendCmd(CMD.calZero()); toast('ตั้งจุดศูนย์แล้ว'); });
    on('btn-cal-high', 'click', () => {
      const ref = parseFloat($('cal-high-ref').value);
      if (isNaN(ref) || ref <= 0) { toast('กรอกค่าลมจริง (> 0)'); return; }
      sendCmd(CMD.calHigh(ref)); toast(`ตั้งจุดสูง @ ${ref} m/s`);
    });
    if (!bleSupported()) { const u = $('unsupported'); if (u) u.classList.remove('hidden'); }
    setConnUI(false);
    if (DEVICE_ID) {
      const dn = $('device-name');
      if (dn) { dn.textContent = 'เครื่อง: ' + DEVICE_ID + ' — กดเชื่อมต่อ'; dn.classList.remove('hidden'); }
    }
    // SW disabled while debugging iOS/Bluefy staleness: actively unregister any
    // existing worker + drop caches so only the no-cache HTTP layer serves assets.
    if ('serviceWorker' in navigator)
      navigator.serviceWorker.getRegistrations().then(rs => rs.forEach(r => r.unregister())).catch(() => {});
    if (window.caches)
      caches.keys().then(ks => ks.forEach(k => caches.delete(k))).catch(() => {});
    // diagnostics: surface missing elements only (stale/partial shell) — silent when ok
    if (_missing.length) showInfo(`${BUILD} · MISSING: ${_missing.join(', ')}`);
  } catch (err) {
    showFatal('init: ' + (err && err.message ? err.message : err));
  }
}

if (document.readyState === 'loading') document.addEventListener('DOMContentLoaded', init);
else init();

})();
