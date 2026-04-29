import React, { useEffect, useState, useCallback } from 'react';
import {
  View, Text, StyleSheet, ScrollView, TouchableOpacity, Switch, Alert, Platform, Modal,
} from 'react-native';
import { useFocusEffect } from '@react-navigation/native';
import DateTimePicker from '@react-native-community/datetimepicker';
import AsyncStorage from '@react-native-async-storage/async-storage';
import { onValue, ref, set } from 'firebase/database';
import { db } from '../firebase';
import { colors, spacing, radius } from '../theme';
import { Ionicons } from '@expo/vector-icons';
import { SCHEDULES_KEY } from '../hooks/useLocalScheduler';

const DEVICES = [
  { key: 'fan',     label: 'พัดลม' },
  { key: 'light',   label: 'ไฟ' },
  { key: 'pump',    label: 'ปั๊ม' },
  { key: 'spray',   label: 'สเปรย์' },
  { key: 'reserve', label: 'สำรอง' },
];

const DAYS = ['อา', 'จ', 'อ', 'พ', 'พฤ', 'ศ', 'ส'];

const emptyForm = () => ({
  id: null,
  target: 'fan',
  startH: 8, startM: 0,
  endH:   9, endM:   0,
  days: [1, 2, 3, 4, 5],
  enabled: true,
});

export default function ScheduleScreen({ route }) {
  const { deviceId } = route.params;
  const [schedules, setSchedules] = useState([]);
  const [appCtrlOn, setAppCtrlOn] = useState(false);
  const [editor, setEditor]       = useState(null);   // form object or null
  const [pickerStart, setPickerStart] = useState(false);
  const [pickerEnd,   setPickerEnd]   = useState(false);

  // ---- NVS schedule (บนบอร์ด, ช่อง /nvsSchedule) ----
  const [nvsSchedule, setNvsSchedule] = useState(null);
  const [showNvs, setShowNvs]         = useState(false);
  const [nvsDay, setNvsDay]           = useState(0);
  const [nvsEdit, setNvsEdit]         = useState(null);
  const [nvsPickStart, setNvsPickStart] = useState(false);
  const [nvsPickEnd,   setNvsPickEnd]   = useState(false);

  // Watch appControlMode from /status (info only)
  useEffect(() => {
    const u = onValue(ref(db, `devices/${deviceId}/status/appControlMode`), (s) => {
      setAppCtrlOn(s.val() === true);
    });
    return u;
  }, [deviceId]);

  // Watch nvsSchedule (ตารางบนบอร์ด)
  useEffect(() => {
    const u = onValue(ref(db, `devices/${deviceId}/nvsSchedule`), (s) => {
      setNvsSchedule(s.val());
    });
    return u;
  }, [deviceId]);

  const load = useCallback(async () => {
    try {
      const raw = await AsyncStorage.getItem(SCHEDULES_KEY(deviceId));
      setSchedules(raw ? JSON.parse(raw) : []);
    } catch (_) { setSchedules([]); }
  }, [deviceId]);

  useFocusEffect(useCallback(() => { load(); }, [load]));

  const save = async (next) => {
    setSchedules(next);
    try { await AsyncStorage.setItem(SCHEDULES_KEY(deviceId), JSON.stringify(next)); }
    catch (e) { Alert.alert('บันทึกไม่สำเร็จ', e.message); }
    // sync to Firebase so the device can run schedules even when app is closed
    try {
      const obj = {};
      next.forEach((s, idx) => {
        obj[String(idx)] = {
          t:  s.target,
          sh: s.startH, sm: s.startM,
          eh: s.endH,   em: s.endM,
          d:  (s.days || []).join(''),
          e:  s.enabled ? 1 : 0,
        };
      });
      await set(ref(db, `devices/${deviceId}/appSchedules`), next.length ? obj : null);
    } catch (e) {
      Alert.alert('Sync Firebase ไม่สำเร็จ', e.message);
    }
  };

  const onAddNew    = () => setEditor(emptyForm());
  const onEditItem  = (it) => setEditor({ ...it });
  const onCancel    = () => setEditor(null);

  const onCommit = async () => {
    if (!editor) return;
    if (!editor.target) { Alert.alert('เลือกอุปกรณ์'); return; }
    if (!editor.days || editor.days.length === 0) {
      Alert.alert('เลือกวัน', 'กรุณาเลือกวันอย่างน้อย 1 วัน'); return;
    }
    const startMin = editor.startH * 60 + editor.startM;
    const endMin   = editor.endH   * 60 + editor.endM;
    if (endMin <= startMin) {
      Alert.alert('เวลาไม่ถูกต้อง', 'เวลาสิ้นสุดต้องอยู่หลังเวลาเริ่ม'); return;
    }
    const item = { ...editor, id: editor.id || Date.now() };
    const next = editor.id
      ? schedules.map((s) => (s.id === editor.id ? item : s))
      : [...schedules, item];
    await save(next);
    setEditor(null);
  };

  const onDelete = (id) => {
    Alert.alert('ลบตารางเวลา?', '', [
      { text: 'ยกเลิก' },
      { text: 'ลบ', style: 'destructive',
        onPress: () => save(schedules.filter((s) => s.id !== id)) },
    ]);
  };

  const onToggleEnabled = (id, current) => {
    save(schedules.map((s) => s.id === id ? { ...s, enabled: !current } : s));
  };

  const toggleDay = (d) => {
    setEditor((p) => {
      if (!p) return p;
      const has = p.days.includes(d);
      return { ...p, days: has ? p.days.filter((x) => x !== d) : [...p.days, d].sort() };
    });
  };

  const targetLabel = (k) => DEVICES.find((d) => d.key === k)?.label || k;

  // ---- NVS edit handlers ----
  const onOpenNvsSlot = (slot, dayIdx, slotIdx) => {
    setNvsEdit({
      d: dayIdx, s: slotIdx,
      hs: slot?.hs ?? 0, ms: slot?.ms ?? 0,
      he: slot?.he ?? 0, me: slot?.me ?? 0,
      fan:     slot?.fan     ? 1 : 0,
      light:   slot?.light   ? 1 : 0,
      pump:    slot?.pump    ? 1 : 0,
      spray:   slot?.spray   ? 1 : 0,
      reserve: slot?.reserve ? 1 : 0,
      state:   slot?.state   ? 1 : 0,
    });
  };

  const saveNvsEdit = async () => {
    if (!nvsEdit) return;
    try {
      const { d, s, hs, ms, he, me, fan, light, pump, spray, reserve, state } = nvsEdit;
      // 1) mirror ค่าใหม่ลง /nvsSchedule/{d}/{s} ทันที เพื่อให้แอพ + Firebase อัปเดตเลย
      //    ไม่ต้องรอ firmware republish (กันกรณีบอร์ดออฟไลน์ชั่วคราว)
      await set(ref(db, `devices/${deviceId}/nvsSchedule/${d}/${s}`), {
        hs, ms, he, me, fan, light, pump, spray, reserve, state,
      });
      // 2) ส่ง scheduleEdit ให้บอร์ดบันทึกลง NVS (one-shot, บอร์ดจะ DELETE เอง)
      await set(ref(db, `devices/${deviceId}/commands/scheduleEdit`), nvsEdit);
      setNvsEdit(null);
      Alert.alert('บันทึกแล้ว', 'อัปเดตค่าใน Firebase และส่งไปยังเครื่องเรียบร้อย');
    } catch (e) {
      Alert.alert('บันทึกไม่สำเร็จ', e.message);
    }
  };

  return (
    <>
    <ScrollView style={styles.container} contentContainerStyle={{ padding: spacing.md }}>
      <View style={[styles.banner, appCtrlOn ? styles.bannerOn : styles.bannerOff]}>
        <Ionicons
          name={appCtrlOn ? 'phone-portrait' : 'phone-portrait-outline'}
          size={20} color={appCtrlOn ? '#1B5E20' : '#B71C1C'}
        />
        <Text style={[styles.bannerText, { color: appCtrlOn ? '#1B5E20' : '#B71C1C' }]}>
          {appCtrlOn
            ? 'App Control Mode เปิดอยู่ — ตารางจะทำงานเมื่อแอพเปิดและถึงเวลา'
            : 'App Control Mode ปิดอยู่ — ตารางจะไม่ทำงาน เปิดที่หน้าหลักก่อน'}
        </Text>
      </View>

      <Text style={styles.note}>
        * ตารางทั้งหมดถูกบันทึกไว้ในมือถือเครื่องนี้เท่านั้น{'\n'}
        * ทำงานเฉพาะตอนเปิดแอพและอยู่ในโหมด App Control{'\n'}
        * เมื่อปิดแอพหรือออฟไลน์ ตารางจะหยุดทำงาน
      </Text>

      <TouchableOpacity style={styles.addBtn} onPress={onAddNew}>
        <Ionicons name="add-circle" size={22} color="#fff" />
        <Text style={styles.addText}>เพิ่มตารางเวลา</Text>
      </TouchableOpacity>

      <Text style={styles.section}>ตารางเวลาทั้งหมด ({schedules.length})</Text>
      {schedules.length === 0 && (
        <Text style={styles.empty}>ยังไม่มีตารางเวลา — กดปุ่มด้านบนเพื่อเพิ่ม</Text>
      )}
      {schedules.map((s) => (
        <TouchableOpacity key={s.id} style={styles.scheduleCard} onPress={() => onEditItem(s)}>
          <View style={{ flex: 1 }}>
            <Text style={styles.schedTitle}>
              {targetLabel(s.target)} · {String(s.startH).padStart(2,'0')}:{String(s.startM).padStart(2,'0')}
              {' → '}
              {String(s.endH).padStart(2,'0')}:{String(s.endM).padStart(2,'0')}
            </Text>
            <Text style={styles.schedSub}>
              {(s.days || []).map((i) => DAYS[i]).join(' ')}
            </Text>
          </View>
          <Switch value={s.enabled} onValueChange={() => onToggleEnabled(s.id, s.enabled)}
            trackColor={{ true: colors.primaryLight }} thumbColor={s.enabled ? colors.primary : '#fff'} />
          <TouchableOpacity onPress={() => onDelete(s.id)} style={{ marginLeft: spacing.sm }}>
            <Ionicons name="trash" size={22} color={colors.danger} />
          </TouchableOpacity>
        </TouchableOpacity>
      ))}

      {/* ===================== NVS Schedule (จากเครื่อง) ===================== */}
      <TouchableOpacity style={styles.nvsHeader} onPress={() => setShowNvs((v) => !v)}>
        <Ionicons name="hardware-chip" size={20} color={colors.primaryDark} />
        <Text style={styles.nvsSectionTitle}>ตารางเวลาบนเครื่อง (NVS)</Text>
        <Ionicons name={showNvs ? 'chevron-up' : 'chevron-down'} size={20} color={colors.primaryDark} />
      </TouchableOpacity>

      {showNvs && (
        <View>
          {nvsSchedule == null ? (
            <Text style={styles.empty}>ยังไม่มีข้อมูล (รอเครื่องส่งหลัง WiFi connect)</Text>
          ) : (
            <>
              <View style={[styles.row, { marginBottom: spacing.sm }]}>
                {DAYS.map((d, i) => (
                  <TouchableOpacity
                    key={i}
                    style={[styles.dayChip, nvsDay === i && styles.chipActive]}
                    onPress={() => setNvsDay(i)}
                  >
                    <Text style={[styles.chipText, nvsDay === i && styles.chipTextActive]}>{d}</Text>
                  </TouchableOpacity>
                ))}
              </View>
              {[0, 1, 2, 3, 4, 5].map((sIdx) => {
                const dayData = nvsSchedule[String(nvsDay)];
                const slot = (dayData && dayData[String(sIdx)]) || {};
                const isActive = slot.state === 1;
                const devs = [];
                if (slot.fan)     devs.push('F');
                if (slot.light)   devs.push('L');
                if (slot.pump)    devs.push('P');
                if (slot.spray)   devs.push('S');
                if (slot.reserve) devs.push('R');
                return (
                  <TouchableOpacity
                    key={sIdx}
                    style={[styles.nvsSlot, isActive && styles.nvsSlotActive]}
                    onPress={() => onOpenNvsSlot(slot, nvsDay, sIdx)}
                  >
                    <Text style={styles.nvsSlotNum}>#{sIdx + 1}</Text>
                    <View style={{ flex: 1, marginLeft: spacing.sm }}>
                      <Text style={[styles.nvsSlotTime, !isActive && styles.nvsSlotOff]}>
                        {`${String(slot.hs ?? 0).padStart(2,'0')}:${String(slot.ms ?? 0).padStart(2,'0')} → ${String(slot.he ?? 0).padStart(2,'0')}:${String(slot.me ?? 0).padStart(2,'0')}`}
                      </Text>
                      {devs.length > 0 && (
                        <Text style={styles.nvsDevices}>{devs.join('  ')}</Text>
                      )}
                    </View>
                    <View style={[styles.nvsBadge, isActive && styles.nvsBadgeOn]}>
                      <Text style={[styles.nvsBadgeText, isActive && styles.nvsBadgeTextOn]}>
                        {isActive ? 'ON' : 'OFF'}
                      </Text>
                    </View>
                    <Ionicons name="create-outline" size={18} color={colors.textMuted} style={{ marginLeft: 6 }} />
                  </TouchableOpacity>
                );
              })}
              <Text style={styles.note}>
                * ตารางนี้เก็บใน NVS ของเครื่อง ทำงานได้แม้ปิดแอพ{'\n'}
                * แตะ slot เพื่อแก้ไขเวลา/อุปกรณ์ → บันทึกลงเครื่อง
              </Text>
            </>
          )}
        </View>
      )}
    </ScrollView>

    {/* ===== NVS Edit Modal ===== */}
    <Modal visible={!!nvsEdit} transparent animationType="fade" onRequestClose={() => setNvsEdit(null)}>
      <View style={styles.modalBackdrop}>
        <ScrollView contentContainerStyle={{ flexGrow: 1, justifyContent: 'center', padding: spacing.md }}>
          <View style={styles.modalCard}>
            <Text style={styles.modalTitle}>
              แก้ไข NVS Slot #{(nvsEdit?.s ?? 0) + 1} · {DAYS[nvsEdit?.d ?? 0]}
            </Text>

            <View style={styles.enableRow}>
              <Text style={styles.label}>เปิดใช้งาน Slot</Text>
              <Switch
                value={nvsEdit?.state === 1}
                onValueChange={(v) => setNvsEdit((p) => ({ ...p, state: v ? 1 : 0 }))}
                trackColor={{ true: colors.primaryLight }}
                thumbColor={nvsEdit?.state ? colors.primary : '#fff'}
              />
            </View>

            <Text style={styles.label}>เวลาเริ่ม</Text>
            <TouchableOpacity style={styles.timeBtn} onPress={() => setNvsPickStart(true)}>
              <Ionicons name="play" size={20} color={colors.primary} />
              <Text style={styles.timeText}>
                {String(nvsEdit?.hs ?? 0).padStart(2,'0')}:{String(nvsEdit?.ms ?? 0).padStart(2,'0')}
              </Text>
            </TouchableOpacity>
            {nvsPickStart && (
              <DateTimePicker
                value={new Date(2024, 0, 1, nvsEdit?.hs ?? 0, nvsEdit?.ms ?? 0)}
                mode="time" is24Hour
                onChange={(e, d) => {
                  setNvsPickStart(Platform.OS === 'ios');
                  if (d) setNvsEdit((p) => ({ ...p, hs: d.getHours(), ms: d.getMinutes() }));
                }}
              />
            )}

            <Text style={styles.label}>เวลาสิ้นสุด</Text>
            <TouchableOpacity style={styles.timeBtn} onPress={() => setNvsPickEnd(true)}>
              <Ionicons name="stop" size={20} color={colors.danger} />
              <Text style={styles.timeText}>
                {String(nvsEdit?.he ?? 0).padStart(2,'0')}:{String(nvsEdit?.me ?? 0).padStart(2,'0')}
              </Text>
            </TouchableOpacity>
            {nvsPickEnd && (
              <DateTimePicker
                value={new Date(2024, 0, 1, nvsEdit?.he ?? 0, nvsEdit?.me ?? 0)}
                mode="time" is24Hour
                onChange={(e, d) => {
                  setNvsPickEnd(Platform.OS === 'ios');
                  if (d) setNvsEdit((p) => ({ ...p, he: d.getHours(), me: d.getMinutes() }));
                }}
              />
            )}

            <Text style={styles.label}>อุปกรณ์</Text>
            <View style={styles.row}>
              {[
                { k: 'fan',     label: 'พัดลม' },
                { k: 'light',   label: 'ไฟ' },
                { k: 'pump',    label: 'ปั๊ม' },
                { k: 'spray',   label: 'สเปรย์' },
                { k: 'reserve', label: 'สำรอง' },
              ].map((d) => {
                const on = nvsEdit?.[d.k] === 1;
                return (
                  <TouchableOpacity
                    key={d.k}
                    style={[styles.chip, on && styles.chipActive]}
                    onPress={() => setNvsEdit((p) => ({ ...p, [d.k]: on ? 0 : 1 }))}
                  >
                    <Text style={[styles.chipText, on && styles.chipTextActive]}>{d.label}</Text>
                  </TouchableOpacity>
                );
              })}
            </View>

            <View style={{ flexDirection: 'row', gap: spacing.sm, marginTop: spacing.md }}>
              <TouchableOpacity style={[styles.modalBtn, styles.modalCancel]} onPress={() => setNvsEdit(null)}>
                <Text style={styles.modalBtnText}>ยกเลิก</Text>
              </TouchableOpacity>
              <TouchableOpacity style={[styles.modalBtn, styles.modalSave]} onPress={saveNvsEdit}>
                <Ionicons name="save" size={18} color="#fff" />
                <Text style={[styles.modalBtnText, { color: '#fff' }]}>บันทึกลงเครื่อง</Text>
              </TouchableOpacity>
            </View>
          </View>
        </ScrollView>
      </View>
    </Modal>

    {/* ===== Editor Modal ===== */}
    <Modal visible={!!editor} transparent animationType="fade" onRequestClose={onCancel}>
      <View style={styles.modalBackdrop}>
        <ScrollView contentContainerStyle={{ flexGrow: 1, justifyContent: 'center', padding: spacing.md }}>
          <View style={styles.modalCard}>
            <Text style={styles.modalTitle}>
              {editor?.id ? 'แก้ไขตารางเวลา' : 'เพิ่มตารางเวลา'}
            </Text>

            <Text style={styles.label}>อุปกรณ์</Text>
            <View style={styles.row}>
              {DEVICES.map((d) => (
                <TouchableOpacity
                  key={d.key}
                  style={[styles.chip, editor?.target === d.key && styles.chipActive]}
                  onPress={() => setEditor((p) => ({ ...p, target: d.key }))}
                >
                  <Text style={[styles.chipText, editor?.target === d.key && styles.chipTextActive]}>
                    {d.label}
                  </Text>
                </TouchableOpacity>
              ))}
            </View>

            <Text style={styles.label}>เวลาเริ่ม</Text>
            <TouchableOpacity style={styles.timeBtn} onPress={() => setPickerStart(true)}>
              <Ionicons name="play" size={20} color={colors.primary} />
              <Text style={styles.timeText}>
                {String(editor?.startH ?? 0).padStart(2,'0')}:{String(editor?.startM ?? 0).padStart(2,'0')}
              </Text>
            </TouchableOpacity>
            {pickerStart && (
              <DateTimePicker
                value={new Date(2024, 0, 1, editor?.startH ?? 0, editor?.startM ?? 0)}
                mode="time" is24Hour
                onChange={(e, d) => {
                  setPickerStart(Platform.OS === 'ios');
                  if (d) setEditor((p) => ({ ...p, startH: d.getHours(), startM: d.getMinutes() }));
                }}
              />
            )}

            <Text style={styles.label}>เวลาสิ้นสุด</Text>
            <TouchableOpacity style={styles.timeBtn} onPress={() => setPickerEnd(true)}>
              <Ionicons name="stop" size={20} color={colors.danger} />
              <Text style={styles.timeText}>
                {String(editor?.endH ?? 0).padStart(2,'0')}:{String(editor?.endM ?? 0).padStart(2,'0')}
              </Text>
            </TouchableOpacity>
            {pickerEnd && (
              <DateTimePicker
                value={new Date(2024, 0, 1, editor?.endH ?? 0, editor?.endM ?? 0)}
                mode="time" is24Hour
                onChange={(e, d) => {
                  setPickerEnd(Platform.OS === 'ios');
                  if (d) setEditor((p) => ({ ...p, endH: d.getHours(), endM: d.getMinutes() }));
                }}
              />
            )}

            <Text style={styles.label}>วันที่ทำงาน</Text>
            <View style={styles.row}>
              {DAYS.map((d, i) => (
                <TouchableOpacity
                  key={i}
                  style={[styles.dayChip, editor?.days?.includes(i) && styles.chipActive]}
                  onPress={() => toggleDay(i)}
                >
                  <Text style={[styles.chipText, editor?.days?.includes(i) && styles.chipTextActive]}>
                    {d}
                  </Text>
                </TouchableOpacity>
              ))}
            </View>

            <View style={styles.enableRow}>
              <Text style={styles.label}>เปิดใช้งาน</Text>
              <Switch
                value={!!editor?.enabled}
                onValueChange={(v) => setEditor((p) => ({ ...p, enabled: v }))}
                trackColor={{ true: colors.primaryLight }}
                thumbColor={editor?.enabled ? colors.primary : '#fff'}
              />
            </View>

            <View style={{ flexDirection: 'row', gap: spacing.sm, marginTop: spacing.md }}>
              <TouchableOpacity style={[styles.modalBtn, styles.modalCancel]} onPress={onCancel}>
                <Text style={styles.modalBtnText}>ยกเลิก</Text>
              </TouchableOpacity>
              <TouchableOpacity style={[styles.modalBtn, styles.modalSave]} onPress={onCommit}>
                <Ionicons name="save" size={18} color="#fff" />
                <Text style={[styles.modalBtnText, { color: '#fff' }]}>บันทึก</Text>
              </TouchableOpacity>
            </View>
          </View>
        </ScrollView>
      </View>
    </Modal>
    </>
  );
}

const styles = StyleSheet.create({
  container: { flex: 1, backgroundColor: colors.background },
  banner: {
    flexDirection: 'row', alignItems: 'center', gap: 8,
    padding: spacing.sm, borderRadius: radius.md, marginBottom: spacing.sm, borderWidth: 1,
  },
  bannerOn:  { backgroundColor: '#E8F5E9', borderColor: '#A5D6A7' },
  bannerOff: { backgroundColor: '#FFEBEE', borderColor: '#EF9A9A' },
  bannerText: { flex: 1, fontWeight: '600', fontSize: 12 },
  note: { color: colors.textMuted, fontSize: 12, marginBottom: spacing.sm, lineHeight: 18 },

  section: { fontSize: 18, fontWeight: '800', color: colors.primaryDark, marginTop: spacing.md, marginBottom: spacing.sm },
  label: { color: colors.text, fontWeight: '600', marginTop: spacing.sm, marginBottom: spacing.xs },
  row: { flexDirection: 'row', flexWrap: 'wrap' },
  chip: {
    backgroundColor: colors.surface, paddingVertical: spacing.sm, paddingHorizontal: spacing.md,
    borderRadius: radius.lg, margin: spacing.xs, borderWidth: 1, borderColor: colors.border,
  },
  dayChip: {
    backgroundColor: colors.surface, padding: spacing.sm,
    borderRadius: radius.lg, margin: spacing.xs, borderWidth: 1, borderColor: colors.border,
    minWidth: 44, alignItems: 'center',
  },
  chipActive: { backgroundColor: colors.primary, borderColor: colors.primary },
  chipText: { color: colors.text, fontWeight: '600' },
  chipTextActive: { color: '#fff' },
  timeBtn: {
    flexDirection: 'row', alignItems: 'center', backgroundColor: colors.surface,
    padding: spacing.md, borderRadius: radius.md, borderWidth: 1, borderColor: colors.border,
  },
  timeText: { fontSize: 22, fontWeight: '700', color: colors.text, marginLeft: spacing.sm },
  enableRow: { flexDirection: 'row', alignItems: 'center', justifyContent: 'space-between', marginTop: spacing.sm },
  addBtn: {
    flexDirection: 'row', alignItems: 'center', justifyContent: 'center',
    backgroundColor: colors.primary, padding: spacing.md, borderRadius: radius.md, gap: spacing.sm,
  },
  addText: { color: '#fff', fontWeight: '700' },
  empty: { textAlign: 'center', color: colors.textMuted, marginTop: spacing.md },
  scheduleCard: {
    flexDirection: 'row', alignItems: 'center', backgroundColor: colors.surface,
    padding: spacing.md, borderRadius: radius.md, marginBottom: spacing.sm,
    borderWidth: 1, borderColor: colors.border,
  },
  schedTitle: { fontWeight: '700', color: colors.text },
  schedSub: { color: colors.textMuted, fontSize: 12, marginTop: 2 },

  // NVS section
  nvsHeader: {
    flexDirection: 'row', alignItems: 'center', backgroundColor: '#E8EAF6',
    padding: spacing.md, borderRadius: radius.md, marginTop: spacing.lg, marginBottom: spacing.sm,
    gap: 8,
  },
  nvsSectionTitle: { flex: 1, fontSize: 15, fontWeight: '700', color: colors.primaryDark },
  nvsSlot: {
    flexDirection: 'row', alignItems: 'center', backgroundColor: colors.surface,
    padding: spacing.sm, borderRadius: radius.sm, marginBottom: 6,
    borderWidth: 1, borderColor: colors.border,
  },
  nvsSlotActive: { backgroundColor: '#E8F5E9', borderColor: '#4CAF50' },
  nvsSlotNum: { fontSize: 13, fontWeight: '700', color: colors.textMuted, width: 28 },
  nvsSlotTime: { fontSize: 14, fontWeight: '700', color: colors.text },
  nvsSlotOff: { color: colors.textMuted },
  nvsDevices: { fontSize: 12, color: colors.primary, fontWeight: '600', marginTop: 2 },
  nvsBadge: {
    paddingHorizontal: 8, paddingVertical: 3, borderRadius: 10,
    backgroundColor: '#EEE', marginLeft: spacing.sm,
  },
  nvsBadgeOn: { backgroundColor: '#4CAF50' },
  nvsBadgeText: { fontSize: 11, fontWeight: '700', color: colors.textMuted },
  nvsBadgeTextOn: { color: '#fff' },

  modalBackdrop: { flex: 1, backgroundColor: 'rgba(0,0,0,0.5)' },
  modalCard: { backgroundColor: colors.background, borderRadius: radius.lg, padding: spacing.md },
  modalTitle: { fontSize: 18, fontWeight: '800', color: colors.primaryDark, marginBottom: spacing.sm, textAlign: 'center' },
  modalBtn: { flex: 1, flexDirection: 'row', alignItems: 'center', justifyContent: 'center', padding: spacing.md, borderRadius: radius.md, gap: 6 },
  modalCancel: { backgroundColor: colors.surface, borderWidth: 1, borderColor: colors.border },
  modalSave: { backgroundColor: colors.primary },
  modalBtnText: { fontWeight: '700', color: colors.text },
});
