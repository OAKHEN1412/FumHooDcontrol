import { initializeApp } from 'firebase/app';
import {
  initializeAuth,
  getReactNativePersistence,
} from 'firebase/auth';
import { getDatabase } from 'firebase/database';
import AsyncStorage from '@react-native-async-storage/async-storage';

// NOTE: API key was previously exposed publicly. Please regenerate
// this key in Firebase Console and update the value below.
const firebaseConfig = {
  apiKey: 'AIzaSyCrx1ygZc24_q3SD-fdhj5FeYdjfNcnv3E',
  authDomain: 'fumhood-ac.firebaseapp.com',
  databaseURL: 'https://fumhood-ac-default-rtdb.asia-southeast1.firebasedatabase.app',
  projectId: 'fumhood-ac',
  storageBucket: 'fumhood-ac.firebasestorage.app',
  messagingSenderId: '557967544756',
  appId: '1:557967544756:web:10ce912abba4ea1638a2e1',
  measurementId: 'G-C4QCQY5DNW',
};

export const app = initializeApp(firebaseConfig);

export const auth = initializeAuth(app, {
  persistence: getReactNativePersistence(AsyncStorage),
});

export const db = getDatabase(app);
