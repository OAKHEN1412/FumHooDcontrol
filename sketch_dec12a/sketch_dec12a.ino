#include <HardwareSerial.h>
#include <esp_system.h>
#define input_alarm_1 9
#define input_alarm_2 10
#define input_alarm_3 11

HardwareSerial mySerial(1);  // UART1: RX=0, TX=1

unsigned long previousMillis = 0;
bool ledState = LOW;
bool isAlarm = false;
unsigned long timer = 0;
bool warring = true;
bool state;

bool alarm1_triggered = false;
bool alarm2_triggered = false;
bool alarm3_triggered = false;

String inputBuffer = "";

// *** debug: พิมพ์สถานะ pin ทุก 2 วินาที ***
unsigned long lastStatusPrint = 0;

// *** 1. เพิ่มตัวแปรสำหรับจำคำสั่งล่าสุดที่จะหน่วงเวลา ***
String pendingFanCommand = "";


void handleHardwareButtons() {
  if (digitalRead(input_alarm_1) == LOW) {
    if (!alarm1_triggered) {
      Serial.println("Alarm_1_ON");
      alarm1_triggered = true;
    }
  } else {
    if (alarm1_triggered) {
      Serial.println("Alarm_1_OFF");
      alarm1_triggered = false;
    }
  }
  if (digitalRead(input_alarm_2) == LOW) {
    if (!alarm2_triggered) {
      Serial.println("Alarm_2_ON");
      alarm2_triggered = true;
    }
  } else {
    if (alarm2_triggered) {
      Serial.println("Alarm_2_OFF");
      alarm2_triggered = false;
    }
  }
  if (digitalRead(input_alarm_3) == LOW) {
    if (!alarm3_triggered) {
      Serial.println("Alarm_3_ON");
      alarm3_triggered = true;
    }
  } else {
    if (alarm3_triggered) {
      Serial.println("Alarm_3_OFF");
      alarm3_triggered = false;
    }
  }
}

void processCommand(String receivedData) {
  if (receivedData.length() == 0) return;

  if (receivedData == "FAN_ON") digitalWrite(2, HIGH);
  else if (receivedData == "FAN_OFF") digitalWrite(2, LOW);
  else if (receivedData == "LIGHT_ON") digitalWrite(3, HIGH);
  else if (receivedData == "LIGHT_OFF") digitalWrite(3, LOW);

  // --- จุดที่แก้ไข ---
  else if (receivedData == "FAN_HIGH") {
    pendingFanCommand = "FAN_HIGH";  // จำไว้ว่าคำสั่งคือเปิด
    timer = millis();                // เริ่มจับเวลา
  } else if (receivedData == "FAN_LOW") {
    pendingFanCommand = "FAN_LOW";  // จำไว้ว่าคำสั่งคือปิด
    timer = millis();               // เริ่มจับเวลา
  }
  // ------------------

  else if (receivedData == "PUMP_ON")digitalWrite(5, HIGH);
  else if (receivedData == "PUMP_OFF") digitalWrite(5, LOW);
  else if (receivedData == "SPRAY_ON") digitalWrite(6, HIGH);
  else if (receivedData == "SPRAY_OFF") digitalWrite(6, LOW);
  else if (receivedData == "RESERVE_ON") digitalWrite(8, HIGH);
  else if (receivedData == "RESERVE_OFF") digitalWrite(8, LOW);
  else if (receivedData == "WIND_LOW") state = true;
  else if (receivedData == "WIND_HIGH") state = false;
  else if (receivedData == "Mute: OPEN") warring = true;
  else if (receivedData == "Mute: CLOSE") warring = false;
  else if (receivedData == "restart") {
    Serial.println("Resetting Arduino...");
    delay(100);
    resetFunc();
  }
}

void handleSerial() {
  while (mySerial.available()) {
    char c = mySerial.read();
    if (c == '\n') {
      inputBuffer.trim();
      Serial.println(inputBuffer);
      processCommand(inputBuffer);
      inputBuffer = "";  // ค่าถูกล้างตรงนี้ ทำให้เอาไปเช็คใน Loop ไม่ได้
    } else if (c != '\r') {
      inputBuffer += c;
    }
  }
}

void handleAlarmBlink() {
  if (isAlarm) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= 500) {
      previousMillis = currentMillis;
      ledState = !ledState;
      digitalWrite(7, ledState);
    }
  } else {
    digitalWrite(7, LOW);
    ledState = LOW;
  }
}

void setup() {
  Serial.begin(19200);
  mySerial.begin(19200, SERIAL_8N1, 0, 1);  // UART1: RX=pin0, TX=pin1
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);  // Pin นี้จะถูกควบคุมโดย FAN_HIGH/LOW
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(input_alarm_1, INPUT);
  pinMode(input_alarm_2, INPUT);
  pinMode(input_alarm_3, INPUT);
}

void loop() {
  handleHardwareButtons();
  handleSerial();

  // 3. จัดการ Timer ของ FAN (แก้ไขแล้ว)
  if (timer != 0 && (millis() - timer >= 3000)) {
    // ใช้ตัวแปร pendingFanCommand ที่เราสร้างมาจำค่าแทน inputBuffer
    if (pendingFanCommand == "FAN_HIGH") {
      Serial.print("on");
      digitalWrite(4, HIGH);
    } else if (pendingFanCommand == "FAN_LOW") {
      Serial.print("off");
      digitalWrite(4, LOW);
    }

    timer = 0;               // รีเซ็ตตัวนับเมื่อทำงานเสร็จแล้ว
    pendingFanCommand = "";  // เคลียร์คำสั่งทิ้ง
  }

  if (state == true && warring == true) {
    isAlarm = true;
  } else {
    isAlarm = false;
  }

  handleAlarmBlink();

  // --- DEBUG: พิมพ์สถานะ pin ทุก 2 วินาที ---
  if (millis() - lastStatusPrint >= 2000) {
    lastStatusPrint = millis();
    Serial.print("[STATUS] A1_pin=");
    Serial.print(digitalRead(input_alarm_1) == LOW ? "LOW(active)" : "HIGH");
    Serial.print(" trig="); Serial.print(alarm1_triggered);
    Serial.print(" | A2_pin=");
    Serial.print(digitalRead(input_alarm_2) == LOW ? "LOW(active)" : "HIGH");
    Serial.print(" trig="); Serial.print(alarm2_triggered);
    Serial.print(" | A3_pin=");
    Serial.print(digitalRead(input_alarm_3) == LOW ? "LOW(active)" : "HIGH");
    Serial.print(" trig="); Serial.println(alarm3_triggered);
  }
}


void resetFunc() {
  esp_restart();
  while(1); // รอจนกว่าเครื่องจะโดนสั่ง Reset
}
