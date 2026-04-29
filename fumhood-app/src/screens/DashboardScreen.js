import React, { useEffect, useState, useRef } from 'react';
import {
  View, Text, StyleSheet, ScrollView, ActivityIndicator,
  Switch, TouchableOpacity, Alert,
} from 'react-native';
import { ref, onValue, update, set } from 'firebase/database';
import { db } from '../firebase';
import { colors, spacing, radius } from '../theme';
import { Ionicons } from '@expo/vector-icons';
import * as Notifications from 'expo-notifications';

Notifications.setNotificationHandler({
  handleNotification: async () => ({
    shouldShowAlert: true,
    shouldPlaySound: true,
    shouldSetBadge: false,
  }),
});

const RELAYS = [
  { key: 'fan',     label: 'พัดลม',  icon: 'sync'        },
  { key: 'light',   label: 'ไฟ',     icon: 'bulb'        },
  { key: 'pump',    label: 'ปั๊ม',   icon: 'water'       },
  { key: 'spray',   label: 'สเปรย์', icon: 'cloud'       },
  { key: 'reserve', label: 'สำรอง',   icon: 'flash'       },
];

const ALARM_LABELS = ['Alarm 1', 'Alarm 2', 'Alarm 3'];
const ALARM_KEYS   = ['alarm1', 'alarm2', 'alarm3'];

export default function DashboardScreen({ route, navigation }) {
  const { deviceId } = route.params;
  const [status, setStatus]           = useState(null);
  const [loading, setLoading]         = useState(true);
  const [lastUpdated, setLastUpdated] = useState(null);
  const [, setTick]                   = useState(0);      // force re-render เพื่ออัปเดต online status
  const lastUpdatedRef = useRef(null);
  const prevAlarmRef   = useRef({ alarm1: false, alarm2: false, alarm3: false });
  const firstUpdateRef = useRef(true);

  useEffect(() => { Notifications.requestPermissionsAsync(); }, []);

  // re-render ทุก 5 วินาที เพื่อให้ online check ใช้ Date.now() ล่าสุดเสมอ
  useEffect(() => {
    const id = setInterval(() => setTick(t => t + 1), 5000);
    return () => clearInterval(id);
  }, []);

  useEffect(() => {
    navigation.setOptions({
      title: `บอร์ด ${deviceId}`,
      headerRight: () => (
        <TouchableOpacity
          onPress={() => navigation.navigate('Schedule', { deviceId })}
          style={{ marginRight: spacing.sm }}
        >
          <Ionicons name="calendar" size={22} color="#fff" />
        </TouchableOpacity>
      ),
    });
  }, [navigation, deviceId]);

  useEffect(() => {
    const u1 = onValue(ref(db, `devices/${deviceId}/status`), (s) => {
      const val = s.val() || {};
      setStatus(val);
      setLoading(false);
      const now = Date.now();
      lastUpdatedRef.current = now;
      setLastUpdated(now);

      if (!firstUpdateRef.current) {
        ALARM_KEYS.forEach((k, i) => {
          const isOn = !!val[k];
          if (isOn && !prevAlarmRef.current[k]) {
            Notifications.scheduleNotificationAsync({
              content: {
                title: `⚠️ ALARM ${ALARM_LABELS[i]} — บอร์ด ${deviceId}`,
                body: `ตรวจพบสัญญาณ ${ALARM_LABELS[i]} จากเครื่อง`,
                sound: true,
              },
              trigger: null,
            });
          }
          prevAlarmRef.current[k] = isOn;
        });
      } else {
        ALARM_KEYS.forEach((k) => { prevAlarmRef.current[k] = !!val[k]; });
        firstUpdateRef.current = false;
      }
    });
    return () => { u1(); };
  }, [deviceId]);

  const online       = status?.online === true && lastUpdated && (Date.now() - lastUpdated < 15000);
  const appCtrlOn    = !!status?.appControlMode;
  const fanOn        = !!status?.fan;
  const silenOn      = !!status?.silen;

  if (loading) {
    return (
      <View style={[styles.container, { justifyContent: 'center' }]}>
        <ActivityIndicator color={colors.primary} size="large" />
      </View>
    );
  }

  const lastSeenStr  = lastUpdated ? new Date(lastUpdated).toLocaleTimeString() : 'N/A';
  const activeAlarms = ALARM_KEYS.filter((k) => !!status?.[k]);
  const directMode   = !!status?.directMode;

  const toggleAppCtrl = () => {
    const next = !appCtrlOn;
    if (!next) {
      Alert.alert('ปิดโหมด App Control?', 'การควบคุมจะกลับไปอยู่ที่ปุ่มกายภาพบนเครื่อง', [
        { text: 'ยกเลิก' },
        { text: 'ปิด', style: 'destructive',
          onPress: () => set(ref(db, `devices/${deviceId}/commands/appControlMode`), false) },
      ]);
    } else {
      set(ref(db, `devices/${deviceId}/commands/appControlMode`), true);
    }
  };

  const setRelay = (key, value) => {
    if (!appCtrlOn) return;
    update(ref(db, `devices/${deviceId}/commands`), { [key]: value });
  };

  const toggleDirect = () => {
    if (!appCtrlOn) return;
    const next = !directMode;
    // เมื่อเข้าโหมด Direct ทุกครั้ง ให้ปิดอุปกรณ์ทั้งหมดก่อน
    if (next) {
      const payload = { directMode: true };
      RELAYS.forEach((r) => { payload[r.key] = false; });
      update(ref(db, `devices/${deviceId}/commands`), payload);
    } else {
      update(ref(db, `devices/${deviceId}/commands`), { directMode: false });
    }
  };

  const clearAlarms = () => {
    set(ref(db, `devices/${deviceId}/commands/clearAlarms`), true);
  };

  const toggleSilen = () => {
    if (!appCtrlOn || !fanOn) return;
    set(ref(db, `devices/${deviceId}/commands/silen`), !silenOn);
  };

  return (
    <ScrollView style={styles.container} contentContainerStyle={{ padding: spacing.md }}>
      <View style={styles.statusBar}>
        <View style={[styles.dot, { backgroundColor: online ? '#4CAF50' : '#F44336' }]} />
        <Text style={styles.statusText}>{online ? 'Online' : 'Offline'}</Text>
        <Text style={styles.lastSeen}>อัปเดตล่าสุด: {lastSeenStr}</Text>
      </View>

      {/* App Control Mode toggle */}
      <View style={[styles.appCtrlCard, appCtrlOn && styles.appCtrlCardOn]}>
        <Ionicons
          name={appCtrlOn ? 'phone-portrait' : 'phone-portrait-outline'}
          size={26}
          color={appCtrlOn ? '#fff' : colors.primaryDark}
        />
        <View style={{ flex: 1, marginLeft: spacing.sm }}>
          <Text style={[styles.appCtrlTitle, appCtrlOn && styles.appCtrlTitleOn]}>
            App Control Mode
          </Text>
          <Text style={[styles.appCtrlHint, appCtrlOn && styles.appCtrlHintOn]}>
            {appCtrlOn
              ? 'แอพควบคุมเครื่องอยู่ ปุ่มกายภาพถูกล็อก'
              : 'เปิดเพื่อสั่งงานจากแอพ (ล็อกปุ่มเครื่อง)'}
          </Text>
        </View>
        <Switch
          value={appCtrlOn}
          onValueChange={toggleAppCtrl}
          trackColor={{ true: '#90CAF9', false: '#999' }}
          thumbColor={appCtrlOn ? '#fff' : '#ddd'}
          disabled={!online}
        />
      </View>

      {/* Mode Direct / Auto */}
      <TouchableOpacity
        style={[styles.modeRow, directMode && styles.modeRowDirect, !appCtrlOn && styles.disabled]}
        onPress={toggleDirect}
        activeOpacity={appCtrlOn ? 0.7 : 1}
      >
        <Ionicons name={directMode ? 'hand-left' : 'time'} size={22} color={directMode ? '#fff' : colors.primaryDark} />
        <View style={{ flex: 1, marginLeft: spacing.sm }}>
          <Text style={[styles.modeLabel, directMode && styles.modeLabelDirect]}>
            {directMode ? 'โหมด Direct (สั่งตรง)' : 'โหมด Auto (ตามตาราง)'}
          </Text>
          <Text style={[styles.modeHint, directMode && styles.modeHintDirect]}>
            {appCtrlOn ? 'แตะเพื่อสลับโหมด' : 'เปิด App Control เพื่อสลับ'}
          </Text>
        </View>
      </TouchableOpacity>

      {activeAlarms.length > 0 && (
        <TouchableOpacity style={styles.alarmBanner} onPress={clearAlarms}>
          <Ionicons name="warning" size={22} color="#fff" />
          <View style={{ flex: 1, marginLeft: spacing.sm }}>
            {activeAlarms.map((k) => (
              <Text key={k} style={styles.alarmText}>
                ⚠️ {ALARM_LABELS[ALARM_KEYS.indexOf(k)]} กำลังทำงาน
              </Text>
            ))}
            <Text style={styles.alarmHint}>แตะเพื่อล้างสัญญาณเตือน</Text>
          </View>
        </TouchableOpacity>
      )}

      <Text style={styles.section}>เซ็นเซอร์ความเร็วลม</Text>
      <View style={styles.row}>
        <SensorCard icon="speedometer" label="m/s"    value={status?.windMs?.toFixed(2)} unit="m/s" />
        <SensorCard icon="speedometer" label="ft/min" value={status?.windFt?.toFixed(1)} unit="ft/min" />
      </View>

      {/* Silen (Mute) toggle */}
      <TouchableOpacity
        style={[styles.silenRow, silenOn && styles.silenRowOn, (!appCtrlOn || !fanOn) && styles.disabled]}
        onPress={toggleSilen}
        activeOpacity={(appCtrlOn && fanOn) ? 0.7 : 1}
      >
        <Ionicons
          name={silenOn ? 'volume-mute' : 'volume-high'}
          size={26}
          color={silenOn ? '#fff' : colors.primaryDark}
        />
        <View style={{ flex: 1, marginLeft: spacing.sm }}>
          <Text style={[styles.silenLabel, silenOn && styles.silenLabelOn]}>
            {silenOn ? 'ปิดเสียงเตือน (Mute)' : 'เปิดเสียงเตือน'}
          </Text>
          <Text style={[styles.silenHint, silenOn && styles.silenHintOn]}>
            {!appCtrlOn
              ? 'เปิด App Control เพื่อสั่งงาน'
              : !fanOn
              ? 'เปิดพัดลมก่อน จึงจะสั่งปิดเสียงได้'
              : 'แตะเพื่อสลับเสียงเตือน'}
          </Text>
        </View>
        <Switch
          value={silenOn}
          onValueChange={toggleSilen}
          trackColor={{ true: '#CE93D8', false: '#999' }}
          thumbColor={silenOn ? '#fff' : '#ddd'}
          disabled={!appCtrlOn || !fanOn || !online}
        />
      </TouchableOpacity>

      <Text style={styles.section}>
        สถานะอุปกรณ์ {appCtrlOn ? '(ควบคุมจากแอพ)' : '(ดูอย่างเดียว)'}
      </Text>
      {RELAYS.map((r) => {
        const val = !!status?.[r.key];
        return (
          <View key={r.key} style={[styles.relayRow, val && styles.relayRowOn]}>
            <Ionicons name={r.icon} size={26} color={val ? '#fff' : colors.primary} />
            <Text style={[styles.relayLabel, val && styles.relayLabelOn]}>{r.label}</Text>
            <Switch
              value={val}
              onValueChange={(v) => setRelay(r.key, v)}
              trackColor={{ true: '#A5D6A7', false: '#999' }}
              thumbColor={val ? '#fff' : '#ddd'}
              disabled={!appCtrlOn || !online}
            />
          </View>
        );
      })}

      <Text style={styles.section}>สถานะ Alarm</Text>
      <View style={styles.row}>
        {ALARM_KEYS.map((k, i) => {
          const on = !!status?.[k];
          return (
            <View key={k} style={[styles.alarmCard, on && styles.alarmCardOn]}>
              <Ionicons name="warning" size={22} color={on ? '#fff' : colors.textMuted} />
              <Text style={[styles.alarmCardText, on && styles.alarmCardTextOn]}>{ALARM_LABELS[i]}</Text>
            </View>
          );
        })}
      </View>

      <TouchableOpacity
        style={styles.scheduleBtn}
        onPress={() => navigation.navigate('Schedule', { deviceId })}
      >
        <Ionicons name="calendar" size={20} color="#fff" />
        <Text style={styles.scheduleBtnText}>ตั้งเวลาการทำงาน</Text>
      </TouchableOpacity>
    </ScrollView>
  );
}

function SensorCard({ icon, label, value, unit }) {
  return (
    <View style={styles.sensorCard}>
      <Ionicons name={icon} size={28} color={colors.primary} />
      <Text style={styles.sensorLabel}>{label}</Text>
      <Text style={styles.sensorValue}>
        {value !== undefined && value !== null ? value : 'N/A'}
        <Text style={styles.sensorUnit}> {unit}</Text>
      </Text>
    </View>
  );
}

const styles = StyleSheet.create({
  container:        { flex: 1, backgroundColor: colors.background },
  statusBar:        { flexDirection: 'row', alignItems: 'center', marginBottom: spacing.sm },
  dot:              { width: 10, height: 10, borderRadius: 5, marginRight: spacing.sm },
  statusText:       { fontWeight: '700', color: colors.text },
  lastSeen:         { marginLeft: spacing.sm, fontSize: 12, color: colors.textMuted },

  appCtrlCard:      { flexDirection: 'row', alignItems: 'center', backgroundColor: '#E3F2FD', padding: spacing.md, borderRadius: radius.md, marginBottom: spacing.md, borderWidth: 2, borderColor: '#BBDEFB' },
  appCtrlCardOn:    { backgroundColor: '#1565C0', borderColor: '#0D47A1' },
  appCtrlTitle:     { fontSize: 15, fontWeight: '800', color: colors.primaryDark },
  appCtrlTitleOn:   { color: '#fff' },
  appCtrlHint:      { fontSize: 11, color: colors.textMuted, marginTop: 2 },
  appCtrlHintOn:    { color: '#BBDEFB' },

  section:          { fontSize: 16, fontWeight: '700', color: colors.primaryDark, marginTop: spacing.md, marginBottom: spacing.sm },
  row:              { flexDirection: 'row', justifyContent: 'space-between', flexWrap: 'wrap' },
  modeRow:          { flexDirection: 'row', alignItems: 'center', backgroundColor: colors.surface, padding: spacing.md, borderRadius: radius.md, marginBottom: spacing.md, borderWidth: 2, borderColor: colors.border },
  modeRowDirect:    { backgroundColor: '#1A237E', borderColor: '#3949AB' },
  modeLabel:        { fontSize: 14, fontWeight: '700', color: colors.text },
  modeLabelDirect:  { color: '#fff' },
  modeHint:         { fontSize: 11, color: colors.textMuted, marginTop: 2 },
  modeHintDirect:   { color: '#90CAF9' },
  disabled:         { opacity: 0.55 },

  silenRow:         { flexDirection: 'row', alignItems: 'center', backgroundColor: colors.surface, padding: spacing.md, borderRadius: radius.md, marginTop: spacing.md, marginBottom: spacing.sm, borderWidth: 2, borderColor: colors.border },
  silenRowOn:       { backgroundColor: '#6A1B9A', borderColor: '#4A148C' },
  silenLabel:       { fontSize: 14, fontWeight: '700', color: colors.text },
  silenLabelOn:     { color: '#fff' },
  silenHint:        { fontSize: 11, color: colors.textMuted, marginTop: 2 },
  silenHintOn:      { color: '#E1BEE7' },

  alarmBanner:      { flexDirection: 'row', alignItems: 'center', backgroundColor: '#D32F2F', padding: spacing.md, borderRadius: radius.md, marginBottom: spacing.md },
  alarmText:        { color: '#fff', fontWeight: '700', fontSize: 13 },
  alarmHint:        { color: '#FFCDD2', fontSize: 11, marginTop: 4 },
  sensorCard:       { flex: 1, backgroundColor: colors.surface, padding: spacing.md, borderRadius: radius.md, margin: spacing.xs, alignItems: 'center', borderWidth: 1, borderColor: colors.border, minWidth: 120 },
  sensorLabel:      { color: colors.textMuted, marginTop: spacing.xs, fontSize: 12 },
  sensorValue:      { fontSize: 22, fontWeight: '800', color: colors.text, marginTop: spacing.xs },
  sensorUnit:       { fontSize: 12, color: colors.textMuted, fontWeight: '500' },
  relayRow:         { flexDirection: 'row', alignItems: 'center', backgroundColor: colors.surface, padding: spacing.md, borderRadius: radius.md, marginBottom: spacing.sm, borderWidth: 1, borderColor: colors.border, gap: spacing.sm },
  relayRowOn:       { backgroundColor: '#2E7D32', borderColor: '#1B5E20' },
  relayLabel:       { flex: 1, fontSize: 15, fontWeight: '600', color: colors.text },
  relayLabelOn:     { color: '#fff' },
  alarmCard:        { flex: 1, backgroundColor: colors.surface, padding: spacing.md, borderRadius: radius.md, margin: spacing.xs, alignItems: 'center', borderWidth: 1, borderColor: colors.border, minWidth: 100 },
  alarmCardOn:      { backgroundColor: '#D32F2F', borderColor: '#B71C1C' },
  alarmCardText:    { color: colors.textMuted, marginTop: spacing.xs, fontSize: 12, fontWeight: '600' },
  alarmCardTextOn:  { color: '#fff' },

  scheduleBtn:      { flexDirection: 'row', alignItems: 'center', justifyContent: 'center', backgroundColor: colors.primary, padding: spacing.md, borderRadius: radius.md, marginTop: spacing.lg, gap: spacing.sm },
  scheduleBtnText:  { color: '#fff', fontSize: 15, fontWeight: '700' },
});
