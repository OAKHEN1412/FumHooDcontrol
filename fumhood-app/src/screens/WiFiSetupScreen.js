import React from 'react';
import { View, Text, StyleSheet, ScrollView, Linking, TouchableOpacity, Share } from 'react-native';
import QRCode from 'react-native-qrcode-svg';
import { Ionicons } from '@expo/vector-icons';
import { colors, spacing, radius } from '../theme';
import { WIFI_PV, buildWifiQrPayload } from '../wifiPv';

export default function WiFiSetupScreen() {
  const payload = buildWifiQrPayload(WIFI_PV);

  const onShare = () =>
    Share.share({
      message:
        `เชื่อมต่อ Wi-Fi: ${WIFI_PV.ssid}\nรหัสผ่าน: ${WIFI_PV.password}\n` +
        `จากนั้นเปิดเว็บ ${WIFI_PV.portalUrl} เพื่อตั้งค่า Wi-Fi บ้าน`,
    });

  return (
    <ScrollView style={styles.container} contentContainerStyle={{ padding: spacing.lg, alignItems: 'center' }}>
      <Text style={styles.title}>ตั้งค่า Wi-Fi ให้บอร์ด</Text>
      <Text style={styles.subtitle}>
        บนบอร์ด: กดปุ่ม ESC ค้างไว้เพื่อเปิดโหมด WiFiPV{'\n'}
        จากนั้นสแกน QR ด้านล่างเพื่อเชื่อมต่อ
      </Text>

      <View style={styles.qrWrap}>
        <QRCode value={payload} size={240} backgroundColor="#fff" color={colors.primaryDark} />
      </View>

      <View style={styles.infoCard}>
        <InfoRow icon="wifi" label="SSID" value={WIFI_PV.ssid} />
        <InfoRow icon="key" label="Password" value={WIFI_PV.password} />
        <InfoRow icon="globe" label="Portal" value={WIFI_PV.portalUrl} />
      </View>

      <TouchableOpacity style={styles.btn} onPress={() => Linking.openURL(WIFI_PV.portalUrl)}>
        <Ionicons name="open-outline" size={20} color="#fff" />
        <Text style={styles.btnText}>เปิดหน้าตั้งค่า ({WIFI_PV.portalUrl})</Text>
      </TouchableOpacity>

      <TouchableOpacity style={[styles.btn, styles.btnSecondary]} onPress={onShare}>
        <Ionicons name="share-outline" size={20} color={colors.primaryDark} />
        <Text style={[styles.btnText, { color: colors.primaryDark }]}>แชร์ข้อมูล Wi-Fi</Text>
      </TouchableOpacity>

      <Text style={styles.steps}>
        วิธีใช้:{'\n'}
        1. ที่บอร์ด ESP32 — กดปุ่ม ESC ค้างไว้ ~3 วินาที{'\n'}
        2. รอจนหน้าจอแสดง “WiFi Setup Mode”{'\n'}
        3. สแกน QR ด้านบนด้วยมือถือ เพื่อเชื่อมต่อ Wi-Fi “{WIFI_PV.ssid}”{'\n'}
        4. เปิดเบราว์เซอร์ไปที่ {WIFI_PV.portalUrl}{'\n'}
        5. เลือก Wi-Fi บ้าน + ใส่รหัสผ่าน → บอร์ดจะรีบูทเอง
      </Text>
    </ScrollView>
  );
}

function InfoRow({ icon, label, value }) {
  return (
    <View style={styles.infoRow}>
      <Ionicons name={icon} size={20} color={colors.primary} />
      <Text style={styles.infoLabel}>{label}</Text>
      <Text style={styles.infoValue}>{value}</Text>
    </View>
  );
}

const styles = StyleSheet.create({
  container: { flex: 1, backgroundColor: colors.background },
  title: { fontSize: 22, fontWeight: '800', color: colors.primaryDark, marginBottom: spacing.sm, textAlign: 'center' },
  subtitle: { color: colors.textMuted, textAlign: 'center', marginBottom: spacing.lg },
  qrWrap: {
    backgroundColor: '#fff', padding: spacing.md, borderRadius: radius.md,
    borderWidth: 2, borderColor: colors.primary, marginBottom: spacing.lg,
  },
  infoCard: {
    width: '100%', backgroundColor: colors.surface, borderRadius: radius.md,
    padding: spacing.md, borderWidth: 1, borderColor: colors.border, marginBottom: spacing.md,
  },
  infoRow: { flexDirection: 'row', alignItems: 'center', paddingVertical: spacing.sm },
  infoLabel: { color: colors.textMuted, marginLeft: spacing.sm, width: 80 },
  infoValue: { color: colors.text, fontWeight: '700', flex: 1 },
  btn: {
    flexDirection: 'row', alignItems: 'center', justifyContent: 'center',
    backgroundColor: colors.primary, padding: spacing.md, borderRadius: radius.md,
    marginTop: spacing.sm, width: '100%',
  },
  btnSecondary: { backgroundColor: colors.surface, borderWidth: 1, borderColor: colors.primary },
  btnText: { color: '#fff', fontWeight: '700', marginLeft: spacing.sm },
  steps: { color: colors.text, marginTop: spacing.lg, lineHeight: 22, alignSelf: 'flex-start' },
});
