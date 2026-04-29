import React, { createContext, useContext, useEffect, useState } from 'react';
import { onAuthStateChanged } from 'firebase/auth';
import { ref, set } from 'firebase/database';
import { auth, db } from './firebase';

const AuthContext = createContext({ user: null, loading: true });

export function AuthProvider({ children }) {
  const [user, setUser] = useState(null);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    const unsub = onAuthStateChanged(auth, async (u) => {
      setUser(u);
      if (u) {
        try {
          await set(ref(db, `users/${u.uid}/profile`), {
            email: u.email || null,
            displayName: u.displayName || null,
            photoURL: u.photoURL || null,
            provider: u.providerData?.[0]?.providerId || null,
            lastLogin: Date.now(),
          });
        } catch (_) { /* rules may block; ignore */ }
      }
      setLoading(false);
    });
    return unsub;
  }, []);

  return (
    <AuthContext.Provider value={{ user, loading }}>
      {children}
    </AuthContext.Provider>
  );
}

export function useAuth() {
  return useContext(AuthContext);
}
