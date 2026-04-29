import React, { useState } from 'react';
import {
  View, Text, TextInput, TouchableOpacity, StyleSheet,
  KeyboardAvoidingView, Platform, Alert, ActivityIndicator, ScrollView,
} from 'react-native';
import { createUserWithEmailAndPassword, updateProfile } from 'firebase/auth';
import { auth } from '../firebase';
import { colors, spacing, radius } from '../theme';
import { Ionicons } from '@expo/vector-icons';

export default function RegisterScreen({ navigation }) {
  const [displayName, setDisplayName] = useState('');
  const [email, setEmail] = useState('');
  const [password, setPassword] = useState('');
  const [confirmPassword, setConfirmPassword] = useState('');
  const [busy, setBusy] = useState(false);
  const [showPassword, setShowPassword] = useState(false);
  const [showConfirm, setShowConfirm] = useState(false);

  const onRegister = async () => {
    if (!displayName || !email || !password || !confirmPassword) {
      Alert.alert('กรอกข้อมูล', 'กรุณากรอกข้อมูลให้ครบ');
      return;
    }
    if (password.length < 6) {
      Alert.alert('รหัสผ่านอ่อนแอ', 'รหัสผ่านต้องมีอย่างน้อย 6 ตัวอักษร');
      return;
    }
    if (password !== confirmPassword) {
      Alert.alert('รหัสผ่านไม่ตรงกัน', 'โปรดตรวจสอบรหัสผ่าน');
      return;
    }

    setBusy(true);
    try {
      const userCred = await createUserWithEmailAndPassword(auth, email.trim(), password);
      await updateProfile(userCred.user, { displayName });
      // Navigation happens automatically via AuthContext
    } catch (e) {
      Alert.alert('สมัครไม่สำเร็จ', e.message);
    } finally {
      setBusy(false);
    }
  };

  return (
    <KeyboardAvoidingView
      behavior={Platform.OS === 'ios' ? 'padding' : undefined}
      style={styles.container}
    >
      <ScrollView contentContainerStyle={styles.inner} keyboardShouldPersistTaps="handled">
        <Text style={styles.title}>สร้างบัญชี</Text>
        <Text style={styles.subtitle}>FumHood</Text>

        <TextInput
          style={styles.input}
          placeholder="ชื่อ"
          value={displayName}
          onChangeText={setDisplayName}
          placeholderTextColor={colors.textMuted}
          editable={!busy}
        />

        <TextInput
          style={styles.input}
          placeholder="อีเมล"
          autoCapitalize="none"
          keyboardType="email-address"
          value={email}
          onChangeText={setEmail}
          placeholderTextColor={colors.textMuted}
          editable={!busy}
        />

        <View style={styles.passwordContainer}>
          <TextInput
            style={styles.passwordInput}
            placeholder="รหัสผ่าน (≥ 6 ตัวอักษร)"
            secureTextEntry={!showPassword}
            value={password}
            onChangeText={setPassword}
            placeholderTextColor={colors.textMuted}
            editable={!busy}
          />
          <TouchableOpacity
            onPress={() => setShowPassword(!showPassword)}
            disabled={!password}
          >
            <Ionicons
              name={showPassword ? 'eye-outline' : 'eye-off-outline'}
              size={20}
              color={colors.primaryDark}
            />
          </TouchableOpacity>
        </View>

        <View style={styles.passwordContainer}>
          <TextInput
            style={styles.passwordInput}
            placeholder="ยืนยันรหัสผ่าน"
            secureTextEntry={!showConfirm}
            value={confirmPassword}
            onChangeText={setConfirmPassword}
            placeholderTextColor={colors.textMuted}
            editable={!busy}
          />
          <TouchableOpacity
            onPress={() => setShowConfirm(!showConfirm)}
            disabled={!confirmPassword}
          >
            <Ionicons
              name={showConfirm ? 'eye-outline' : 'eye-off-outline'}
              size={20}
              color={colors.primaryDark}
            />
          </TouchableOpacity>
        </View>

        <TouchableOpacity style={[styles.button, busy && { opacity: 0.6 }]} onPress={onRegister} disabled={busy}>
          {busy ? <ActivityIndicator color="#fff" /> : <Text style={styles.buttonText}>สร้างบัญชี</Text>}
        </TouchableOpacity>

        <TouchableOpacity onPress={() => navigation.goBack()} disabled={busy}>
          <Text style={styles.link}>มีบัญชีแล้ว? เข้าสู่ระบบ</Text>
        </TouchableOpacity>
      </ScrollView>
    </KeyboardAvoidingView>
  );
}

const styles = StyleSheet.create({
  container: { flex: 1, backgroundColor: colors.background },
  inner: { flexGrow: 1, padding: spacing.lg, justifyContent: 'center' },
  title: { fontSize: 28, fontWeight: '800', color: colors.primaryDark, textAlign: 'center' },
  subtitle: { fontSize: 14, color: colors.primary, textAlign: 'center', marginBottom: spacing.lg, letterSpacing: 2 },
  input: {
    backgroundColor: colors.surface,
    borderRadius: radius.md,
    padding: spacing.md,
    marginBottom: spacing.md,
    borderWidth: 1,
    borderColor: colors.border,
    color: colors.text,
  },
  passwordContainer: {
    flexDirection: 'row',
    alignItems: 'center',
    backgroundColor: colors.surface,
    borderRadius: radius.md,
    paddingHorizontal: spacing.md,
    marginBottom: spacing.md,
    borderWidth: 1,
    borderColor: colors.border,
  },
  passwordInput: {
    flex: 1,
    padding: spacing.md,
    color: colors.text,
  },
  button: {
    backgroundColor: colors.primary,
    padding: spacing.md,
    borderRadius: radius.md,
    alignItems: 'center',
    marginTop: spacing.sm,
  },
  buttonText: { color: '#fff', fontSize: 16, fontWeight: '700' },
  link: { color: colors.primaryDark, textAlign: 'center', marginTop: spacing.lg, fontWeight: '600' },
});
