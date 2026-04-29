import { useEffect, useRef } from 'react';
import AsyncStorage from '@react-native-async-storage/async-storage';
import { ref, update } from 'firebase/database';
import { db } from '../firebase';

export const SCHEDULES_KEY = (deviceId) => `schedules:${deviceId}`;

// Local scheduler — schedules are stored in AsyncStorage on the phone.
// While the app is open and App Control mode is on, this hook checks
// every 20s and fires relay on/off commands when the current minute
// matches a schedule's start/end time.
export default function useLocalScheduler(deviceId, appCtrlOn, online) {
  const firedRef = useRef({});

  useEffect(() => {
    if (!deviceId) return;
    let cancelled = false;

    const tick = async () => {
      if (cancelled) return;
      if (!appCtrlOn || !online) return;
      let list = [];
      try {
        const raw = await AsyncStorage.getItem(SCHEDULES_KEY(deviceId));
        list = raw ? JSON.parse(raw) : [];
      } catch (_) { return; }

      const now = new Date();
      const day = now.getDay(); // 0=Sun..6=Sat
      const hh  = now.getHours();
      const mm  = now.getMinutes();
      const dateKey = now.toISOString().slice(0, 10);

      list.forEach((s) => {
        if (!s || !s.enabled) return;
        if (!Array.isArray(s.days) || !s.days.includes(day)) return;
        if (!s.target) return;

        if (s.startH === hh && s.startM === mm) {
          const k = `${s.id}:${dateKey}:${hh}:${mm}:on`;
          if (!firedRef.current[k]) {
            firedRef.current[k] = true;
            update(ref(db, `devices/${deviceId}/commands`), { [s.target]: true })
              .catch(() => {});
          }
        }
        if (s.endH === hh && s.endM === mm) {
          const k = `${s.id}:${dateKey}:${hh}:${mm}:off`;
          if (!firedRef.current[k]) {
            firedRef.current[k] = true;
            update(ref(db, `devices/${deviceId}/commands`), { [s.target]: false })
              .catch(() => {});
          }
        }
      });

      // garbage-collect old fired keys (keep only today's)
      const keys = Object.keys(firedRef.current);
      if (keys.length > 200) {
        const next = {};
        keys.forEach((k) => { if (k.includes(`:${dateKey}:`)) next[k] = true; });
        firedRef.current = next;
      }
    };

    tick();
    const id = setInterval(tick, 20000);
    return () => { cancelled = true; clearInterval(id); };
  }, [deviceId, appCtrlOn, online]);
}
