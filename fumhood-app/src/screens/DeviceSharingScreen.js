import React, { useState, useEffect } from 'react';
import {
  View, Text, TextInput, TouchableOpacity, StyleSheet,
  FlatList, Alert, ActivityIndicator, Modal, KeyboardAvoidingView, Platform,
} from 'react-native';
import { ref, onValue, set, remove } from 'firebase/database';
import { db } from '../firebase';
import { useAuth } from '../AuthContext';
import { colors, spacing, radius } from '../theme';
import { Ionicons } from '@expo/vector-icons';

export default function DeviceSharingScreen({ navigation, route }) {
  const { deviceId } = route.params;
  const { user } = useAuth();
  const [sharedUsers, setSharedUsers] = useState([]);
  const [loading, setLoading] = useState(true);
  const [modalVisible, setModalVisible] = useState(false);
  const [userEmail, setUserEmail] = useState('');
  const [sharing, setSharing] = useState(false);

  useEffect(() => {
    const ownersRef = ref(db, `devices/${deviceId}/owners`);
    const unsub = onValue(ownersRef, (snap) => {
      const owners = snap.val() || {};
      // Load user details for each owner
      const ownerList = Object.entries(owners).map(([uid]) => ({ uid }));
      setSharedUsers(ownerList);
      setLoading(false);
    });
    return () => unsub();
  }, [deviceId]);

  const onShareDevice = async () => {
    if (!userEmail.trim()) {
      Alert.alert('กรอกอีเมล', 'กรุณากรอกอีเมลของผู้ใช้ที่ต้องการแชร์');
      return;
    }

    setSharing(true);
    try {
      // TODO: In future, implement user lookup by email
      // For now, just show a placeholder
      Alert.alert('ข้อมูล', 'ฟีเจอร์นี้อยู่ระหว่างพัฒนา\n\nเร็วๆ นี้จะสามารถแชร์ได้โดยป้อนอีเมล');
      
      // Placeholder: แชร์ให้ user โดยใช้ email แล้วสร้าง UID (ต้องเพิ่ม backend)
      // await set(ref(db, `devices/${deviceId}/owners/${newUserId}`), true);
      
      setUserEmail('');
      setModalVisible(false);
    } catch (e) {
      Alert.alert('แชร์ไม่สำเร็จ', e.message);
    } finally {
      setSharing(false);
    }
  };

  const onRemoveAccess = (uid) => {
    if (uid === user.uid) {
      Alert.alert('ไม่สามารถลบตัวเอง', 'คุณไม่สามารถลบสิทธิ์ของตัวเองได้');
      return;
    }

    Alert.alert('ลบสิทธิ์การเข้าถึง', 'ยืนยันการลบสิทธิ์ของผู้ใช้นี้?', [
      { text: 'ยกเลิก' },
      {
        text: 'ลบ',
        style: 'destructive',
        onPress: async () => {
          try {
            await remove(ref(db, `devices/${deviceId}/owners/${uid}`));
            Alert.alert('สำเร็จ', 'ลบสิทธิ์เสร็จแล้ว');
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
        <TouchableOpacity onPress={() => navigation.goBack()}>
          <Ionicons name="chevron-back" size={28} color={colors.primary} />
        </TouchableOpacity>
        <Text style={styles.title}>จัดการการแชร์ {deviceId}</Text>
        <TouchableOpacity onPress={() => setModalVisible(true)}>
          <Ionicons name="person-add" size={24} color={colors.primary} />
        </TouchableOpacity>
      </View>

      {/* Users list */}
      {loading ? (
        <ActivityIndicator color={colors.primary} style={styles.loader} />
      ) : sharedUsers.length === 0 ? (
        <View style={styles.empty}>
          <Ionicons name="people" size={64} color={colors.textMuted} />
          <Text style={styles.emptyText}>ยังไม่มีผู้ใช้รายอื่น</Text>
        </View>
      ) : (
        <FlatList
          data={sharedUsers}
          keyExtractor={(item) => item.uid}
          renderItem={({ item }) => (
            <View style={styles.userItem}>
              <View style={styles.userInfo}>
                <Ionicons name="person-circle" size={48} color={colors.primary} />
                <View style={styles.userDetails}>
                  <Text style={styles.userName}>{item.uid}</Text>
                  <Text style={styles.userRole}>
                    {item.uid === user.uid ? '(ตัวคุณ - เจ้าของ)' : 'ผู้ใช้ที่แชร์ให้'}
                  </Text>
                </View>
              </View>
              {item.uid !== user.uid && (
                <TouchableOpacity
                  style={styles.removeBtn}
                  onPress={() => onRemoveAccess(item.uid)}
                >
                  <Ionicons name="close-circle" size={28} color={colors.error} />
                </TouchableOpacity>
              )}
            </View>
          )}
        />
      )}

      {/* Modal for adding user */}
      <Modal visible={modalVisible} transparent animationType="slide">
        <KeyboardAvoidingView
          behavior={Platform.OS === 'ios' ? 'padding' : undefined}
          style={styles.modalOverlay}
        >
          <View style={styles.modalContent}>
            <View style={styles.modalHeader}>
              <Text style={styles.modalTitle}>เพิ่มผู้ใช้</Text>
              <TouchableOpacity onPress={() => setModalVisible(false)}>
                <Ionicons name="close" size={28} color={colors.text} />
              </TouchableOpacity>
            </View>

            <TextInput
              style={styles.input}
              placeholder="อีเมลของผู้ใช้"
              keyboardType="email-address"
              autoCapitalize="none"
              value={userEmail}
              onChangeText={setUserEmail}
              placeholderTextColor={colors.textMuted}
              editable={!sharing}
            />

            <TouchableOpacity
              style={[styles.button, sharing && { opacity: 0.6 }]}
              onPress={onShareDevice}
              disabled={sharing}
            >
              {sharing ? (
                <ActivityIndicator color="#fff" />
              ) : (
                <Text style={styles.buttonText}>แชร์อุปกรณ์</Text>
              )}
            </TouchableOpacity>

            <Text style={styles.info}>
              💡 ใส่อีเมลของผู้ใช้ที่ต้องการแชร์อุปกรณ์นี้ให้
            </Text>
          </View>
        </KeyboardAvoidingView>
      </Modal>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: colors.background,
  },
  header: {
    flexDirection: 'row',
    alignItems: 'center',
    justifyContent: 'space-between',
    paddingHorizontal: spacing.md,
    paddingVertical: spacing.md,
    backgroundColor: colors.surface,
    borderBottomWidth: 1,
    borderBottomColor: colors.border,
  },
  title: {
    fontSize: 18,
    fontWeight: 'bold',
    color: colors.text,
  },
  loader: {
    marginTop: spacing.lg,
  },
  empty: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
  },
  emptyText: {
    marginTop: spacing.md,
    fontSize: 16,
    color: colors.textMuted,
  },
  userItem: {
    flexDirection: 'row',
    alignItems: 'center',
    justifyContent: 'space-between',
    paddingHorizontal: spacing.md,
    paddingVertical: spacing.md,
    borderBottomWidth: 1,
    borderBottomColor: colors.border,
  },
  userInfo: {
    flexDirection: 'row',
    alignItems: 'center',
    flex: 1,
  },
  userDetails: {
    marginLeft: spacing.md,
    flex: 1,
  },
  userName: {
    fontSize: 16,
    fontWeight: '600',
    color: colors.text,
  },
  userRole: {
    fontSize: 12,
    color: colors.textMuted,
    marginTop: 4,
  },
  removeBtn: {
    padding: spacing.sm,
  },
  modalOverlay: {
    flex: 1,
    backgroundColor: 'rgba(0,0,0,0.5)',
    justifyContent: 'flex-end',
  },
  modalContent: {
    backgroundColor: colors.surface,
    paddingHorizontal: spacing.md,
    paddingVertical: spacing.lg,
    borderTopLeftRadius: radius.lg,
    borderTopRightRadius: radius.lg,
  },
  modalHeader: {
    flexDirection: 'row',
    justifyContent: 'space-between',
    alignItems: 'center',
    marginBottom: spacing.lg,
  },
  modalTitle: {
    fontSize: 18,
    fontWeight: 'bold',
    color: colors.text,
  },
  input: {
    borderWidth: 1,
    borderColor: colors.border,
    borderRadius: radius.md,
    paddingHorizontal: spacing.md,
    paddingVertical: spacing.sm,
    marginBottom: spacing.md,
    color: colors.text,
    fontSize: 16,
  },
  button: {
    backgroundColor: colors.primary,
    borderRadius: radius.md,
    paddingVertical: spacing.md,
    alignItems: 'center',
    marginBottom: spacing.md,
  },
  buttonText: {
    color: '#fff',
    fontSize: 16,
    fontWeight: '600',
  },
  info: {
    fontSize: 14,
    color: colors.textMuted,
    textAlign: 'center',
  },
});
