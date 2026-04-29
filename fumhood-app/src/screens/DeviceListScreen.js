import React, { useState } from 'react';
import {
  View, Text, TextInput, TouchableOpacity, StyleSheet, FlatList,
  Alert, ActivityIndicator, Modal, KeyboardAvoidingView, Platform,
} from 'react-native';
import { useEffect } from 'react';
import { ref, onValue, set, get, remove } from 'firebase/database';
import { signOut } from 'firebase/auth';
import { auth, db } from '../firebase';
import { useAuth } from '../AuthContext';
import { colors, spacing, radius } from '../theme';
import { Ionicons } from '@expo/vector-icons';

export default function DeviceListScreen({ navigation }) {
  const { user } = useAuth();
  const [devices, setDevices] = useState([]);
  const [loading, setLoading] = useState(true);
  const [modalVisible, setModalVisible] = useState(false);
  const [pairId, setPairId] = useState('');
  const [pairing, setPairing] = useState(false);

  useEffect(() => {
    if (!user) return;
    const userDevicesRef = ref(db, `users/${user.uid}/devices`);
    const unsub = onValue(userDevicesRef, (snap) => {
      const v = snap.val() || {};
      setDevices(Object.entries(v).map(([id, meta]) => ({ id, ...meta })));
      setLoading(false);
    });
    return () => unsub();
  }, [user]);

  const openModal = () => {
    setPairId('');
    setModalVisible(true);
  };

  const onPair = async () => {
    const id = pairId.trim().toUpperCase();
    if (id.length !== 6) {
      Alert.alert('รหัสไม่ถูกต้อง', 'กรุณากรอกรหัส 6 หลัก (ตัวอักษรและตัวเลข)');
      return;
    }
    if (!/^[A-Z0-9]{6}$/.test(id)) {
      Alert.alert('รหัสไม่ถูกต้อง', 'รหัสต้องประกอบด้วยตัวอักษร A-Z และตัวเลข 0-9 เท่านั้น');
      return;
    }
    // ตรวจสอบว่าเพิ่มซ้ำหรือไม่
    if (devices.some((d) => d.id === id)) {
      Alert.alert('มีอยู่แล้ว', `อุปกรณ์ ${id} ถูกเพิ่มไว้แล้ว`);
      return;
    }
    setPairing(true);
    try {
      // ตรวจสอบว่าบอร์ดนี้มีในฐานข้อมูลจริงหรือไม่
      const snap = await get(ref(db, `devices/${id}`));
      if (!snap.exists()) {
        Alert.alert(
          'ไม่พบอุปกรณ์',
          `ไม่พบบอร์ดที่มีรหัส "${id}" ในระบบ\n\nตรวจสอบให้แน่ใจว่าบอร์ดเชื่อมต่ออินเทอร์เน็ตแล้ว`,
        );
        setPairing(false);
        return;
      }
      // บันทึก device ลิงก์กับ user
      await set(ref(db, `devices/${id}/owner`), user.uid);
      await set(ref(db, `users/${user.uid}/devices/${id}`), {
        name: id, addedAt: Date.now(),
      });
      setModalVisible(false);
    } catch (e) {
      Alert.alert('เพิ่มอุปกรณ์ไม่สำเร็จ', e.message);
    }
    setPairing(false);
  };

  const onRemove = (id) => {
    Alert.alert('ลบอุปกรณ์', `ต้องการลบ ${id}?`, [
      { text: 'ยกเลิก' },
      {
        text: 'ลบ', style: 'destructive', onPress: async () => {
          try {
            await remove(ref(db, `users/${user.uid}/devices/${id}`));
          } catch (e) {
            Alert.alert('ลบไม่สำเร็จ', e.message);
          }
        },
      },
    ]);
  };

  return (
    <View style={styles.container}>
      {/* Header */}
      <View style={styles.header}>
        <View>
          <Text style={styles.headerTitle}>อุปกรณ์ของฉัน</Text>
          <Text style={styles.headerSub}>{user?.email}</Text>
        </View>
        <TouchableOpacity onPress={() => signOut(auth)}>
          <Ionicons name="log-out-outline" size={28} color={colors.primaryDark} />
        </TouchableOpacity>
      </View>

      {/* Buttons row */}
      <View style={styles.btnRow}>
        <TouchableOpacity style={styles.addDeviceBtn} onPress={openModal}>
          <Ionicons name="add-circle" size={20} color="#fff" />
          <Text style={styles.addDeviceBtnText}>เพิ่มอุปกรณ์</Text>
        </TouchableOpacity>
        <TouchableOpacity
          style={styles.wifiBtn}
          onPress={() => navigation.navigate('WiFiSetup')}
        >
          <Ionicons name="wifi" size={20} color="#fff" />
          <Text style={styles.wifiBtnText}>ตั้งค่า WiFi</Text>
        </TouchableOpacity>
      </View>

      {/* Device list */}
      {loading ? (
        <ActivityIndicator color={colors.primary} style={{ marginTop: spacing.lg }} />
      ) : (
        <FlatList
          data={devices}
          keyExtractor={(d) => d.id}
          ListEmptyComponent={
            <View style={styles.emptyBox}>
              <Ionicons name="hardware-chip-outline" size={48} color={colors.textMuted} />
              <Text style={styles.empty}>ยังไม่มีอุปกรณ์</Text>
              <Text style={styles.emptySub}>กด "เพิ่มอุปกรณ์" แล้วกรอกรหัส 6 หลักจากบอร์ด</Text>
            </View>
          }
          renderItem={({ item }) => (
            <TouchableOpacity
              style={styles.card}
              onPress={() => navigation.navigate('Dashboard', { deviceId: item.id })}
              onLongPress={() => onRemove(item.id)}
            >
              <Ionicons name="hardware-chip" size={28} color={colors.primary} />
              <View style={{ flex: 1, marginLeft: spacing.md }}>
                <Text style={styles.cardTitle}>{item.name || item.id}</Text>
                <Text style={styles.cardSub}>ID: {item.id}</Text>
              </View>
              <Ionicons name="chevron-forward" size={22} color={colors.textMuted} />
            </TouchableOpacity>
          )}
        />
      )}

      {/* Add Device Modal */}
      <Modal
        visible={modalVisible}
        transparent
        animationType="slide"
        onRequestClose={() => setModalVisible(false)}
      >
        <KeyboardAvoidingView
          style={styles.overlay}
          behavior={Platform.OS === 'ios' ? 'padding' : undefined}
        >
          <View style={styles.modalBox}>
            <View style={styles.modalHeader}>
              <Text style={styles.modalTitle}>เพิ่มอุปกรณ์</Text>
              <TouchableOpacity onPress={() => setModalVisible(false)}>
                <Ionicons name="close" size={24} color={colors.textMuted} />
              </TouchableOpacity>
            </View>

            {/* คำอธิบาย */}
            <View style={styles.infoBox}>
              <Ionicons name="information-circle" size={18} color={colors.primary} />
              <Text style={styles.infoText}>
                กด ESC ค้าง 3 วินาทีบนบอร์ด{'\n'}
                จะแสดงรหัส 6 หลักสีเหลืองบนหน้าจอ
              </Text>
            </View>

            <Text style={styles.inputLabel}>รหัสอุปกรณ์ (6 หลัก)</Text>
            <TextInput
              style={styles.input}
              placeholder="เช่น A1B2C3"
              autoCapitalize="characters"
              autoCorrect={false}
              maxLength={6}
              value={pairId}
              onChangeText={(t) => setPairId(t.replace(/[^A-Za-z0-9]/g, '').toUpperCase())}
              placeholderTextColor={colors.textMuted}
              keyboardType="default"
            />
            <Text style={styles.charCount}>{pairId.length}/6</Text>

            <TouchableOpacity
              style={[styles.confirmBtn, (pairId.length !== 6 || pairing) && { opacity: 0.5 }]}
              onPress={onPair}
              disabled={pairId.length !== 6 || pairing}
            >
              {pairing
                ? <ActivityIndicator color="#fff" />
                : <>
                    <Ionicons name="link" size={18} color="#fff" />
                    <Text style={styles.confirmBtnText}>จับคู่อุปกรณ์</Text>
                  </>
              }
            </TouchableOpacity>
          </View>
        </KeyboardAvoidingView>
      </Modal>
    </View>
  );
}

const styles = StyleSheet.create({
  container: { flex: 1, backgroundColor: colors.background, padding: spacing.md },
  header: { flexDirection: 'row', justifyContent: 'space-between', alignItems: 'center', marginBottom: spacing.md },
  headerTitle: { fontSize: 22, fontWeight: '800', color: colors.primaryDark },
  headerSub: { fontSize: 12, color: colors.textMuted },
  btnRow: { flexDirection: 'row', gap: spacing.sm, marginBottom: spacing.md },
  addDeviceBtn: {
    flex: 1, flexDirection: 'row', alignItems: 'center', justifyContent: 'center',
    backgroundColor: colors.primary, padding: spacing.md, borderRadius: radius.md,
  },
  addDeviceBtnText: { color: '#fff', fontWeight: '700', marginLeft: spacing.sm },
  wifiBtn: {
    flex: 1, flexDirection: 'row', alignItems: 'center', justifyContent: 'center',
    backgroundColor: colors.accent ?? '#1976D2', padding: spacing.md, borderRadius: radius.md,
  },
  wifiBtnText: { color: '#fff', fontWeight: '700', marginLeft: spacing.sm },
  emptyBox: { alignItems: 'center', marginTop: spacing.xl * 2 },
  empty: { fontSize: 16, fontWeight: '700', color: colors.textMuted, marginTop: spacing.md },
  emptySub: { fontSize: 13, color: colors.textMuted, textAlign: 'center', marginTop: spacing.sm },
  card: {
    flexDirection: 'row', alignItems: 'center', backgroundColor: colors.surface,
    padding: spacing.md, borderRadius: radius.md, marginBottom: spacing.sm,
    borderWidth: 1, borderColor: colors.border,
  },
  cardTitle: { fontSize: 16, fontWeight: '700', color: colors.text },
  cardSub: { fontSize: 12, color: colors.textMuted },
  // --- Modal ---
  overlay: { flex: 1, backgroundColor: 'rgba(0,0,0,0.5)', justifyContent: 'flex-end' },
  modalBox: {
    backgroundColor: colors.surface ?? '#fff', borderTopLeftRadius: 20, borderTopRightRadius: 20,
    padding: spacing.lg, paddingBottom: spacing.xl,
  },
  modalHeader: { flexDirection: 'row', justifyContent: 'space-between', alignItems: 'center', marginBottom: spacing.md },
  modalTitle: { fontSize: 18, fontWeight: '800', color: colors.primaryDark },
  infoBox: {
    flexDirection: 'row', alignItems: 'flex-start', backgroundColor: colors.background,
    padding: spacing.md, borderRadius: radius.md, marginBottom: spacing.md, gap: spacing.sm,
  },
  infoText: { flex: 1, fontSize: 13, color: colors.text, lineHeight: 20 },
  inputLabel: { fontSize: 14, fontWeight: '600', color: colors.text, marginBottom: spacing.sm },
  input: {
    backgroundColor: colors.background, borderRadius: radius.md, padding: spacing.md,
    borderWidth: 2, borderColor: colors.primary, color: colors.text,
    fontSize: 24, fontWeight: '800', textAlign: 'center', letterSpacing: 8,
  },
  charCount: { textAlign: 'right', fontSize: 12, color: colors.textMuted, marginBottom: spacing.md },
  confirmBtn: {
    flexDirection: 'row', alignItems: 'center', justifyContent: 'center',
    backgroundColor: colors.primary, padding: spacing.md, borderRadius: radius.md,
  },
  confirmBtnText: { color: '#fff', fontWeight: '700', fontSize: 16, marginLeft: spacing.sm },
});
