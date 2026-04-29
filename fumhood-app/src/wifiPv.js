// WiFi Provisioning AP credentials shown to the user as a QR code.
// Must match the SSID/password used by WiFiManager on the ESP32-S3.
export const WIFI_PV = {
  ssid: 'Fumhood_Setup',
  password: '12345678',
  // Default IP exposed by WiFiManager AP captive portal
  portalUrl: 'http://192.168.4.1',
};

// Build a Wi-Fi QR code payload that most phones can auto-connect to.
// Format: WIFI:T:WPA;S:<ssid>;P:<password>;;
export function buildWifiQrPayload({ ssid, password, hidden = false }) {
  const escape = (s) => String(s).replace(/([\\;,":])/g, '\\$1');
  return `WIFI:T:WPA;S:${escape(ssid)};P:${escape(password)};H:${hidden ? 'true' : 'false'};;`;
}
