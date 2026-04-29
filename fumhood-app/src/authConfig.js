// Google OAuth Client IDs from Firebase Console / Google Cloud Console
//
// HOW TO GET THESE:
// 1. Firebase Console → Authentication → Sign-in method → Enable Google
// 2. Click the Google provider → expand "Web SDK configuration"
//    → copy "Web client ID" → paste into webClientId below.
// 3. For Android: Firebase Console → Project Settings → Your apps → Add Android app
//    (use applicationId from app.json e.g. com.yourname.fumhood)
//    → it auto-creates an OAuth client. Copy the Android client ID → androidClientId.
// 4. For iOS: similarly add iOS app → copy iOS client ID → iosClientId.
//
// For development with Expo Go, only `webClientId` is required (works in Expo Go).
// For standalone/EAS builds you also need android/ios IDs.

export const GOOGLE_OAUTH = {
  // Required for Expo Go and all builds
  webClientId: '557967544756-mvbtqv3baksa6rv1a1qe5tnu60auftgj.apps.googleusercontent.com',
  // For standalone Android builds via EAS:
  // firebase console → project settings → add android app → copy android client id
  // androidClientId: 'YOUR_ANDROID_CLIENT_ID',
  // For standalone iOS builds via EAS:
  // From GoogleService-Info.plist → CLIENT_ID
  iosClientId: '557967544756-7io9lviv16336q5j1c7n2hj8sj4d6pc3.apps.googleusercontent.com',
};

// List of email addresses treated as Admins. Admins use Email/Password login.
// Regular users sign in via Google.
// (Also enforced via /admins/{uid} node in Realtime Database.)
export const ADMIN_EMAILS = [
  'oak@gmail.com',
];
