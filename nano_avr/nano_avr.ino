// FumHood Nano (ATmega328P) — AVR port of sketch_dec12a (originally ESP32 code).
// Single hardware UART (D0/D1) = link to the main ESP32-S3 board's Serial2, 19200 8N1.
//   S3 -> Nano : FAN_ON/OFF, LIGHT_ON/OFF, FAN_HIGH/LOW, PUMP_ON/OFF, SPRAY_ON/OFF,
//                RESERVE_ON/OFF, WIND_LOW/HIGH, "Mute: OPEN" / "Mute: CLOSE", restart
//   Nano -> S3 : Alarm_1/2/3_ON / _OFF  (read by Read_nano() on the S3)
// Outputs: 2=FAN 3=LIGHT 4=FAN speed(HIGH/LOW, applied 3s after cmd) 5=PUMP 6=SPRAY
//          7=alarm buzzer/LED blink  8=RESERVE
// Inputs (active LOW): 9,10,11 = alarm 1,2,3
//
// NOTE: D0/D1 are shared with the USB(FT232) programming line. If upload fails to sync,
//       disconnect the S3 TX from the Nano RX (D0) during flashing.

#define input_alarm_1 9
#define input_alarm_2 10
#define input_alarm_3 11

unsigned long previousMillis = 0;
bool ledState = LOW;
bool isAlarm = false;
unsigned long timer = 0;
bool warring = true;
bool state = false;

bool alarm1_triggered = false;
bool alarm2_triggered = false;
bool alarm3_triggered = false;

// Buzzer (PASSIVE) on D12, driven with tone() so it actually sounds.
// pin7 = RELAY contact for an EXTERNAL sounder. When enabled (extBuzz), the relay
// MIRRORS the D12 buzzer pattern on/off (it can't carry tone(), only the on/off
// envelope). Enabled/disabled from the web via BLE ("EXTBUZZ:0/1"), stored in S3 NVS.
#define BUZZ_PIN  12   // passive buzzer (tone)
#define RELAY_PIN 7    // relay for external sounder (digitalWrite on/off)

bool extBuzz = false;  // external sounder enable — synced from S3 ("EXTBUZZ:n"), NVS-backed on S3

// buzzer alert pattern — set by S3 via "BUZZ:n", default slow blink
//   0 = ต่อเนื่อง (continuous ON)   1 = กระพริบช้า 500ms   2 = กระพริบเร็ว 150ms   3 = บี๊บคู่
int buzzMode = 1;
int buzzPhase = 0;   // double-beep state

// Manual buzzer test from S3/serial: "TESTBUZZ:n" forces pattern n regardless of
// the alarm inputs; "TESTBUZZ:off" stops it. Lets the buzzer be bench-tested.
bool testAlarm = false;

// Buzzer drive frequency (Hz). Tune to the passive buzzer's resonant freq for max
// loudness (commonly ~2000-2700Hz).
#define BUZZ_FREQ 2300

// Drive the D12 passive buzzer on/off via tone() (an AC waveform — required for
// passive buzzers; a plain digitalWrite HIGH stays silent). The pin7 relay
// mirrors the same on/off level when the external sounder is enabled.
void setBuzz(bool on) {
  if (on) tone(BUZZ_PIN, BUZZ_FREQ);
  else { noTone(BUZZ_PIN); digitalWrite(BUZZ_PIN, LOW); }
  digitalWrite(RELAY_PIN, (extBuzz && on) ? HIGH : LOW);
}

String inputBuffer = "";
String pendingFanCommand = "";

// AVR soft reset: jump to address 0 (replaces ESP32 esp_restart()).
void resetFunc() { asm volatile ("jmp 0"); }

void handleHardwareButtons() {
  if (digitalRead(input_alarm_1) == LOW) {
    if (!alarm1_triggered) { Serial.println("Alarm_1_ON");  alarm1_triggered = true;  }
  } else {
    if (alarm1_triggered)  { Serial.println("Alarm_1_OFF"); alarm1_triggered = false; }
  }
  if (digitalRead(input_alarm_2) == LOW) {
    if (!alarm2_triggered) { Serial.println("Alarm_2_ON");  alarm2_triggered = true;  }
  } else {
    if (alarm2_triggered)  { Serial.println("Alarm_2_OFF"); alarm2_triggered = false; }
  }
  if (digitalRead(input_alarm_3) == LOW) {
    if (!alarm3_triggered) { Serial.println("Alarm_3_ON");  alarm3_triggered = true;  }
  } else {
    if (alarm3_triggered)  { Serial.println("Alarm_3_OFF"); alarm3_triggered = false; }
  }
}

void processCommand(String receivedData) {
  if (receivedData.length() == 0) return;
  if      (receivedData == "FAN_ON")      digitalWrite(2, HIGH);
  else if (receivedData == "FAN_OFF")     digitalWrite(2, LOW);
  else if (receivedData == "LIGHT_ON")    digitalWrite(3, HIGH);
  else if (receivedData == "LIGHT_OFF")   digitalWrite(3, LOW);
  else if (receivedData == "FAN_HIGH")  { pendingFanCommand = "FAN_HIGH"; timer = millis(); }
  else if (receivedData == "FAN_LOW")   { pendingFanCommand = "FAN_LOW";  timer = millis(); }
  else if (receivedData == "PUMP_ON")     digitalWrite(5, HIGH);
  else if (receivedData == "PUMP_OFF")    digitalWrite(5, LOW);
  else if (receivedData == "SPRAY_ON")    digitalWrite(6, HIGH);
  else if (receivedData == "SPRAY_OFF")   digitalWrite(6, LOW);
  else if (receivedData == "RESERVE_ON")  digitalWrite(8, HIGH);
  else if (receivedData == "RESERVE_OFF") digitalWrite(8, LOW);
  else if (receivedData == "WIND_LOW")    state = true;
  else if (receivedData == "WIND_HIGH")   state = false;
  else if (receivedData == "Mute: OPEN")  warring = true;
  else if (receivedData == "Mute: CLOSE") warring = false;
  else if (receivedData.startsWith("BUZZ:")) {
    int m = receivedData.substring(5).toInt();
    if (m < 0) m = 0; if (m > 3) m = 3;
    buzzMode = m;
  }
  else if (receivedData.startsWith("EXTBUZZ:")) {
    int v = receivedData.substring(8).toInt();
    extBuzz = (v != 0);
    if (!extBuzz) digitalWrite(RELAY_PIN, LOW);   // disable -> drop relay now
  }
  else if (receivedData.startsWith("TESTBUZZ:")) {
    String a = receivedData.substring(9);
    if (a == "off" || a == "OFF" || a == "x") { testAlarm = false; }
    else {
      int m = a.toInt();
      if (m < 0) m = 0; if (m > 3) m = 3;
      buzzMode = m; buzzPhase = 0; testAlarm = true;
    }
  }
  else if (receivedData == "restart")     resetFunc();
}

void handleSerial() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      inputBuffer.trim();
      processCommand(inputBuffer);
      inputBuffer = "";
    } else if (c != '\r') {
      inputBuffer += c;
    }
  }
}

void handleAlarmBlink() {
  if (!isAlarm) {
    setBuzz(false);   // also drives pin7 relay LOW
    ledState = LOW;
    buzzPhase = 0;
    previousMillis = millis();
    return;
  }
  unsigned long now = millis();
  switch (buzzMode) {
    case 0:  // ต่อเนื่อง
      setBuzz(true);
      ledState = HIGH;
      break;
    case 2:  // กระพริบเร็ว 150ms
      if (now - previousMillis >= 150) { previousMillis = now; ledState = !ledState; setBuzz(ledState); }
      break;
    case 3: {  // บี๊บคู่: on120 off120 on120 off600
      static const unsigned int durs[4] = {120, 120, 120, 600};
      static const uint8_t   lvls[4] = {HIGH, LOW, HIGH, LOW};
      setBuzz(lvls[buzzPhase]);
      if (now - previousMillis >= durs[buzzPhase]) { previousMillis = now; buzzPhase = (buzzPhase + 1) & 3; }
      break;
    }
    case 1:  // กระพริบช้า 500ms (default)
    default:
      if (now - previousMillis >= 500) { previousMillis = now; ledState = !ledState; setBuzz(ledState); }
      break;
  }
}

void setup() {
  Serial.begin(19200);   // hardware UART D0/D1 = link to ESP32-S3 (Serial2)
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(BUZZ_PIN, OUTPUT);   // D12 passive buzzer
  pinMode(input_alarm_1, INPUT);
  pinMode(input_alarm_2, INPUT);
  pinMode(input_alarm_3, INPUT);
}

void loop() {
  handleHardwareButtons();
  handleSerial();

  // FAN speed relay (pin 4): apply 3s after the FAN_HIGH/LOW command arrives
  if (timer != 0 && (millis() - timer >= 3000)) {
    if      (pendingFanCommand == "FAN_HIGH") digitalWrite(4, HIGH);
    else if (pendingFanCommand == "FAN_LOW")  digitalWrite(4, LOW);
    timer = 0;
    pendingFanCommand = "";
  }

  isAlarm = (state == true && warring == true) || testAlarm;
  handleAlarmBlink();
}
