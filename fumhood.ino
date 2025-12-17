#include <Arduino.h>
#include <Wire.h>
// #include "button_nav.h"
#include <RTClib.h>
#include <lvgl.h>
#include "LGFX_ILI9488_S3.hpp"
#include "ui.h"      // SquareLine export
#include "memory.h"
LGFX tft;

#define RX_PIN 37 // รับข้อมูลจาก Nano
#define TX_PIN 36 // ส่งข้อมูลไป Nano
/* ---------------- LVGL DRAW BUFFER ---------------- */
#define SCREEN_WIDTH   480
#define SCREEN_HEIGHT  320
#define DRAW_BUF_LINES 40

static lv_color_t draw_buf[SCREEN_WIDTH * DRAW_BUF_LINES];

/* ---------------- FLUSH FUNCTION (LVGL v9) ---------------- */
void my_disp_flush( lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p )
{
    uint32_t w = ( area->x2 - area->x1 + 1 );
    uint32_t h = ( area->y2 - area->y1 + 1 );

    tft.startWrite();
    tft.setAddrWindow( area->x1, area->y1, w, h );
    tft.pushColors( ( uint16_t * )&color_p->full, w * h, true );
    tft.endWrite();

    lv_disp_flush_ready( disp );
}
/* ---------------- OPTIONAL TICK ---------------- */
static uint32_t my_tick(void)
{
    return millis();
}
/* ---------------- PIN DEFINITIONS ---------------- */
#define bt_1  5
#define bt_2  6
#define bt_3  7
#define bt_4  15
#define bt_5  16
#define bt_6  3
#define bt_7  46
#define bt_8  13
#define bt_9  14
#define bt_10 38

// 7 segment 3digit
#include <ShiftRegister74HC595.h>
ShiftRegister74HC595 sr(3, 39, 40, 41);
int value, digit1, digit2, digit3;
uint8_t numberB[] = {
  B11000000,  // 0
  B11111001,  // 1
  B10100100,  // 2
  B10110000,  // 3
  B10011001,  // 4
  B10010010,  // 5
  B10000010,  // 6
  B11111000,  // 7
  B10000000,  // 8
  B10010000   // 9
};


RTC_DS3231 rtc;
const int OutPin = 4;

float windms;
float windMS;
// --- Wind Sensor Variables ---
float windFtPerMin = 0;

// --- Filtering / Averaging ---
const int numSamples = 20;        // ปรับได้
float windmsValues[numSamples];
int sampleIndex = 0;
bool filled = false;

// --- Alert Settings ---
float alert_set = 1.0;            // ตั้งค่าเตือนตามต้องการ

// --- Mode and States ---
String mode = "auto";
String mode_senser = "ms";
bool silen = false;

// --- Display / 7-Segment ---
int windsensor = 0;
// --- Pins ---
const int led1 = 12;
const int led2 = 13;
const int relay_1 = 26;
const int relay_2 = 27;
int stat1, stat2, stat3, stat4, stat5, stat6, stat7, stat8, stat9, stat10;
int btn_home, btn_time, btn_light, btn_fan, btn_high, btn_pump, btn_spray, btn_esc, btn_ent, btn_mute;
// --- Time / RTC ---
char timeStr[10];
char ddmmyy[12];
String dayweek;
bool needRefresh = true;
// ต้องมีฟังก์ชันนี้
String dayOfWeek(int index) {
  switch (index) {
    case 0: return "Sun";
    case 1: return "Mon";
    case 2: return "Tue";
    case 3: return "Wed";
    case 4: return "Thu";
    case 5: return "Fri";
    case 6: return "Sat";
  }
  return "";
}
// Filter buffer

float samples[numSamples];
int sampleIndex2 = 0;
bool bufferFilled = false;


float getFilteredWind(float newValue) {
    samples[sampleIndex2] = newValue;
    sampleIndex2++;

    if (sampleIndex2 >= numSamples) {
        sampleIndex2 = 0;
        bufferFilled = true;
    }

    int count = bufferFilled ? numSamples : sampleIndex2;

    float sum = 0;
    for (int i = 0; i < count; i++) {
        sum += samples[i];
    }

    return sum / count;
}
bool lastEscState1 = false;
bool lastEscState = false;

bool escPressedOnce() {
    bool state = (btn_esc == HIGH);

    if(state && !lastEscState) {
        lastEscState = state;
        return true;       
    }

    lastEscState = state;
    return false;          
}
void updateLabels(int idx, ProgramData &x) {
    if (idx < 0 || idx > 5) return; 

    lv_obj_t* h_labels[] = { objects.h_label1, objects.h_label2, objects.h_label3, objects.h_label4, objects.h_label5, objects.h_label6 };
    lv_obj_t* d_labels[] = { objects.d_label1, objects.d_label2, objects.d_label3, objects.d_label4, objects.d_label5, objects.d_label6 };

    char timeRange[32];
    sprintf(timeRange, "%02d:%02d\n%02d:%02d", 
            x.hour_start[idx], x.minute_start[idx], 
            x.hour_end[idx], x.minute_end[idx]);
    
    // แล้วนำ timeRange ไปใส่ใน Label ที่ต้องการ
    String dTxt = String(system(x.device[idx])); 

    lv_label_set_text(h_labels[idx], timeRange);
    lv_label_set_text(d_labels[idx], dTxt.c_str());
}

// ฟังก์ชันนี้เรียกทีเดียว วนลูปจบเลย
void updateAllLabels(ProgramData &x) {
    for(int i=0; i<6; i++) {
        updateLabels(i, x);
    }
}
void digit(int senser) {

  int digit1 = senser / 100;
  int digit2 = (senser / 10) % 10;
  int digit3 = senser % 10;

  uint8_t seg3 = numberB[digit1] & B01111111;  // เปิดจุดทศนิยม
  uint8_t seg2 = numberB[digit2];
  uint8_t seg1 = numberB[digit3];

  uint8_t numberToPrint[] = { seg3, seg2, seg1 };
  sr.setAll(numberToPrint);
}

unsigned long previousMillisWind = 0;
const unsigned long windInterval = 800; // ดีเลย์ 1 วินาที
char wind[32];
char windft[32];
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
bool warring = true;
bool is_wind_zero_state = false; 
void readWind() {   // ← ทำให้เป็น void
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillisWind >= windInterval) {
        previousMillisWind = currentMillis;

        int windADunits = analogRead(OutPin);   // Raw ADC
        float windMPH = pow((((float)windADunits - 264.0) / 85.6814), 3.36814);
        windMS = windMPH * 0.44704;
        windms = mapFloat(windMS, 9699.95, 13173.06, 0.60, 2.00);   
      windFtPerMin = windms * 3.28084 * 60;
      // เก็บค่าใน array
      windmsValues[sampleIndex] = windms;
      sampleIndex++;

      if (sampleIndex >= numSamples) {
        sampleIndex = 0;
        filled = true;
      }

    }
    float filteredWind = getFilteredWind(windms);

if (filteredWind > alert_set + 0.5) {
    digitalWrite(led2, LOW);
    silen = true;
    digitalWrite(led1, HIGH);

    if (mode != "direct") {
        digitalWrite(relay_1, HIGH);
        digitalWrite(relay_2, LOW);
    }
}

if (mode_senser == "ms") {
    windsensor = windms * 100;
    if (windsensor <= -1) {
        digit(0);
        lv_label_set_text(objects.senser, "0.00 M/s");
        lv_label_set_text(objects.senser_1, "0.00 M/s");
        if (is_wind_zero_state == false) { 
            Serial2.println("WIND_LOW");
            is_wind_zero_state = true;
        }
        
    } 
   if (windsensor > 60) {
        if (is_wind_zero_state == true) {
            Serial2.println("WIND_HIGH");
            delay(200);
            is_wind_zero_state = false; 
        }
        if (windsensor > 999) {
            sr.setAllLow();
        } else {
            digit(windsensor);
            lv_snprintf(wind, sizeof(wind), "%.2f M/s", windms);
            lv_label_set_text(objects.senser, wind);
            lv_label_set_text(objects.senser_1, wind);
        }
    }
}
if (mode_senser == "ft") {
    windsensor = windms * 100;

    if (windFtPerMin > 999) {
        sr.setAllLow();
    }
    if (windsensor > 0 && windFtPerMin <= 999) {
        digitft(windFtPerMin);
        lv_snprintf(windft, sizeof(windft), "%.2f FT/m", windFtPerMin);
        lv_label_set_text(objects.senser, windft);
        lv_label_set_text(objects.senser_1, windft);
    }
    if (windsensor <= 0) {
        digitft(0);
        lv_label_set_text(objects.senser, "0 FT/m");
        lv_label_set_text(objects.senser_1, "0 FT/m");
    }
}
}
// --- Global Variables ---
bool isMuteOpen = true;
int lastBtnState = LOW;
unsigned long pressStartTime = 0;
bool isLongPressHandled = false;

void handle_mute_button() {
    int currentBtnState = digitalRead(bt_10);
    if (currentBtnState == HIGH && lastBtnState == LOW) {
        pressStartTime = millis();
        isLongPressHandled = false; 
        delay(20); 
    }
    if (currentBtnState == HIGH) {
        if ((millis() - pressStartTime >= 3000) && isLongPressHandled == false) {
            
            if (mode_senser == "ms") {
                mode_senser = "ft";
                Serial2.println("Mode: FT");
            } else {
                mode_senser = "ms";
                Serial2.println("Mode: MS");
            }
            isLongPressHandled = true;
            while(digitalRead(bt_10) == HIGH) {
                delay(10); 
            }
        }
    }
    if (currentBtnState == LOW && lastBtnState == HIGH) {
        
        unsigned long pressDuration = millis() - pressStartTime;
        if (pressDuration > 50 && pressDuration < 3000 && isLongPressHandled == false) {
            
            isMuteOpen = !isMuteOpen;
            
           if (isMuteOpen) {
                warring = true; 
                lv_obj_clear_state(objects.sound_1, LV_STATE_CHECKED);
                lv_obj_clear_state(objects.sound_2, LV_STATE_CHECKED);
                
                Serial2.println("Mute: OPEN"); 
            } else {
                warring = false;
                lv_obj_add_state(objects.sound_1, LV_STATE_CHECKED);
                lv_obj_add_state(objects.sound_2, LV_STATE_CHECKED);
                
                Serial2.println("Mute: CLOSE"); 
            }
            lv_obj_invalidate(objects.sound_1);
            lv_obj_invalidate(objects.sound_2);
        }
        delay(50);
    }
    lastBtnState = currentBtnState;
}
void digitft(int senser) {

  int digit1 = senser / 100;
  int digit2 = (senser / 10) % 10;
  int digit3 = senser % 10;

  uint8_t seg3 = numberB[digit1];  // เปิดจุดทศนิยม
  uint8_t seg2 = numberB[digit2];
  uint8_t seg1 = numberB[digit3];

  uint8_t numberToPrint[] = { seg3, seg2, seg1 };
  sr.setAll(numberToPrint);
}

void timer() {
    DateTime now = rtc.now();
    int dayOfWeekIndex = now.dayOfTheWeek();

    sprintf(timeStr, "%02d:%02d", now.hour(), now.minute());
    sprintf(ddmmyy, "%02d/%02d/%04d", now.day(), now.month(), now.year());
    dayweek = dayOfWeek(dayOfWeekIndex);

    lv_label_set_text(objects.timer, timeStr);
    lv_label_set_text(objects.timer1, timeStr);
    lv_label_set_text(objects.dd_mm_yy_1, ddmmyy);
    lv_label_set_text(objects.dd_mm_yy_2, ddmmyy);

   
}

void bt() {
  static unsigned long lastPress = 0;

  if (millis() - lastPress > 200) {
    lastPress = millis();
    btn_home = digitalRead(bt_1);
    btn_time = digitalRead(bt_2);
    btn_light = digitalRead(bt_3);
    btn_fan = digitalRead(bt_4);
    btn_high = digitalRead(bt_5);
    btn_pump = digitalRead(bt_7);
    btn_spray = digitalRead(bt_6);
    btn_esc = digitalRead(bt_8);
    btn_ent = digitalRead(bt_9);
    btn_mute = digitalRead(bt_10);

    if (btn_home == 1) { stat1++; }
    if (btn_time == 1) { stat2++; }
    if (btn_light == 1) { stat3++; }
    if (btn_fan == 1) { stat4++; }
    if (btn_high == 1) { stat5++; }
    if (btn_pump == 1) { stat6++; }
    if (btn_spray == 1) { stat7++; }
    if (btn_esc == 1) { stat8++; }
    if (stat8 > 1) { stat8 = 0; }
    if (btn_ent == 1) { stat9++; }
    if (stat9 > 1) { stat9 = 0; }
    if (btn_mute == 1) { stat10++; }
  }

  // Serial.print(stat1);
  // Serial.print(" : ");
  // Serial.print(stat2);
  // Serial.print(" : ");
  // Serial.print(stat3);
  // Serial.print(" : ");
  // Serial.print(stat4);
  // Serial.print(" : ");
  // Serial.print(stat5);
  // Serial.print(" : ");
  // Serial.print(stat6);
  // Serial.print(" : ");
  // Serial.print(stat7);
  // Serial.print(" : ");
  // Serial.println(stat8);
  //  Serial.print(" : ");
  //  Serial.println(stat9);
  //  Serial.print(" : ");
  //  Serial.println(stat10);
}

void sync_state(lv_obj_t *src, lv_obj_t *dst, lv_state_t state) {

    // ส่วนที่ 1: Sync สถานะระหว่างปุ่ม (คงเดิม)
    if(lv_obj_has_state(src, state)){
        lv_obj_add_state(dst, state);
    }
    else{
        lv_obj_clear_state(dst, state);
    }
    
    // ส่วนที่ 2: เช็คสถานะเพื่อส่ง Serial (ส่งเฉพาะเมื่อมีการเปลี่ยนแปลง)
    
    // สร้างตัวแปร static เพื่อจำสถานะเก่าไว้ (ค่าจะไม่หายเมื่อจบฟังก์ชัน)
    static bool prev_fan = false;
    static bool prev_light = false;
    static bool prev_pump = false;
    static bool prev_spray = false;

    // --- FAN ---
    bool curr_fan = lv_obj_has_state(objects.fan, LV_STATE_CHECKED); // อ่านค่าปัจจุบัน
    if (curr_fan != prev_fan) {  // ถ้าค่าปัจจุบัน "ไม่เหมือน" ค่าเก่า (มีการกดเปลี่ยน)
        if(curr_fan) {
            Serial2.println("FAN_ON");
        } else {
            Serial2.println("FAN_OFF");
        }
        prev_fan = curr_fan; // จำค่าใหม่ไว้ใช้รอบหน้า
    }

    // --- LIGHT ---
    bool curr_light = lv_obj_has_state(objects.light, LV_STATE_CHECKED);
    if (curr_light != prev_light) {
        if(curr_light) {
            Serial2.println("LIGHT_ON");
        } else {
            Serial2.println("LIGHT_OFF");
        }
        prev_light = curr_light;
    }

    // --- PUMP ---
    bool curr_pump = lv_obj_has_state(objects.pump, LV_STATE_CHECKED);
    if (curr_pump != prev_pump) {
        if(curr_pump) {
            Serial2.println("PUMP_ON");
        } else {
            Serial2.println("PUMP_OFF");
        }
        prev_pump = curr_pump;
    }

    // --- SPRAY ---
    bool curr_spray = lv_obj_has_state(objects.spray, LV_STATE_CHECKED);
    if (curr_spray != prev_spray) {
        if(curr_spray) {
            Serial2.println("SPRAY_ON");
        } else {
            Serial2.println("SPRAY_OFF");
        }
        prev_spray = curr_spray;
    }

}

// ฟังก์ชันช่วย toggle state ของ object ตาม stat
void toggle_obj_state(lv_obj_t *obj, int &stat) {
    if(stat == 1) {
        lv_obj_add_state(obj, LV_STATE_CHECKED);
    } else if(stat > 1) {
        lv_obj_clear_state(obj, LV_STATE_CHECKED);
        stat = 0; // รีเซ็ต stat
    }
}

static unsigned long pressStart = 0;
static bool switched = false;

void checkModeSensor() {
    int btn = digitalRead(bt_10);

    if (btn == HIGH) {
        if (!switched) pressStart = pressStart ? pressStart : millis();
        if (millis() - pressStart >= 2000 && !switched) {
            mode_senser = (mode_senser == "ms") ? "ft" : "ms";
            switched = true;
        }
    } else {
        pressStart = 0;
        switched = false;
    }
}


/* ---------------- GLOBAL ---------------- */
int curIndex = 0;               // ปุ่มที่ highlight ปัจจุบัน
bool pgWorkLoaded = false;      // เช็คหน้า pg_work
unsigned long lastPressTime[3] = {0,0,0};
const unsigned long debounceDelay = 200;

int findNext(int curIndex, int dx, int dy) {
    const int buttonMap[3][3] = {
        {0, 1, 2},
        {3, 4, 5},
        {-1, 6, -1}
    };
    
    int x = -1, y = -1;
    for(int row=0; row<3; row++){
        for(int col=0; col<3; col++){
            if(buttonMap[row][col] == curIndex) { x = col; y = row; break; }
        }
        if(x != -1) break;
    }
    if(x == -1) return curIndex;

    int nx = x, ny = y;

    // loop until find a valid button
    for(int i=0; i<3; i++){      // สูงสุด 3 แถว
        ny = (ny + dy + 3) % 3;
        nx = (nx + dx + 3) % 3;

        if(buttonMap[ny][nx] != -1)
            return buttonMap[ny][nx];
    }

    return curIndex; // ถ้าไม่เจออะไร
}
/* ===================== NEW VARIABLES ===================== */
int currentIndex = 0;                 // ปุ่มที่เลือกอยู่ (แทน curIndex)
int previousIndex = -1;               // ปุ่มก่อนหน้า
int selectedButton = -1;              // ปุ่มล่าสุดที่เปลี่ยน
bool changedFlag = false;             // flag สำหรับเช็คว่าเปลี่ยนปุ่มหรือไม่
int edit_slot_id = 0;
unsigned long lastPressTimeNew[4] = {0,0,0,0};
const unsigned long keyDelay = 200;
bool wait_for_release = false;
/* ===================== BUTTON LAYOUT 3x3 ===================== */
const int buttonMatrix[3][2] = {
    {0, 1},  // row 0
    {2, 3},  // row 1
    {4, 5}   // row 2
};
/* ===================== FIND NEXT BUTTON ===================== */
int findNext3x2(int curIndex, int dx, int dy) {

    int x = -1, y = -1;

    // หา (x,y) ของปุ่มปัจจุบัน
    for(int row=0; row<3; row++){
        for(int col=0; col<2; col++){
            if(buttonMatrix[row][col] == curIndex){
                y = row;
                x = col;
                break;
            }
        }
        if(x != -1) break;
    }

    if(x == -1) return curIndex;

    int nx = x;
    int ny = y;

    // วนหาได้สูงสุด 3*2=6 ครั้ง
    for(int i = 0; i < 6; i++){
        nx = (nx + dx + 2) % 2;   // 2 columns
        ny = (ny + dy + 3) % 3;   // 3 rows

        if(buttonMatrix[ny][nx] != -1)
            return buttonMatrix[ny][nx];
    }

    return curIndex;
}
// --- Matrix สำหรับหน้าตั้งค่า (3 แถว, 2 คอลัมน์) ---
const int settingsMatrix[3][2] = {
    {0, 1}, // Row 0: Hour Start, Min Start
    {2, 3}, // Row 1: Hour End, Min End
    {4, 5}  // Row 2: Device, Save Button
};

int findNextSettings(int curIdx, int dx, int dy) {
    int x = -1, y = -1;
    // หาตำแหน่งปัจจุบัน
    for(int r=0; r<3; r++){
        for(int c=0; c<2; c++){
            if(settingsMatrix[r][c] == curIdx) { y=r; x=c; break; }
        }
    }
    if(x == -1) return curIdx;

    // คำนวณตำแหน่งใหม่
    int nx = (x + dx + 2) % 2; // วนลูปแนวนอน (2 คอลัมน์)
    int ny = (y + dy + 3) % 3; // วนลูปแนวตั้ง (3 แถว)

    return settingsMatrix[ny][nx];
}

String Day_pg;
// --- วางฟังก์ชันนี้ไว้ด้านบน ก่อนถึง CT_mode() ---

void updateAllSwitches(ProgramData &x) {
    // รวม Object ของ Switch ทั้ง 6 ตัวเข้า Array
    lv_obj_t* sw_targets[] = {
        objects.state_swich1, objects.state_swich2, objects.state_swich3,
        objects.state_swich4, objects.state_swich5, objects.state_swich6
    };

    for (int i = 0; i < 6; i++) {
        // กัน Error กรณี Object เป็น NULL
        if(sw_targets[i] == NULL) continue; 

        if (x.state[i] == 1) {
            // ถ้าใน memory เป็น 1 ให้แสดงสถานะเปิด (Checked)
            lv_obj_add_state(sw_targets[i], LV_STATE_CHECKED);
        } else {
            // ถ้าเป็น 0 ให้แสดงสถานะปิด
            lv_obj_clear_state(sw_targets[i], LV_STATE_CHECKED);
        }
        // บังคับวาดใหม่ทันที
        lv_obj_invalidate(sw_targets[i]);
    }
}
/* ===================== UPDATE BUTTON SELECTION ===================== */
void updatePgSwitch() {
    lv_label_set_text(objects.pg_switch_day, Day_pg.c_str());

    int keyLeft  = digitalRead(bt_3);
    int keyRight = digitalRead(bt_4);
    int keyDown  = digitalRead(bt_7);
    int keyUp    = digitalRead(bt_6);
    unsigned long now = millis();

    int newIndex = currentIndex;

    if(keyLeft == HIGH && now - lastPressTimeNew[0] > keyDelay){
        newIndex = findNext3x2(currentIndex, -1, 0);
        lastPressTimeNew[0] = now;
    }
    if(keyRight == HIGH && now - lastPressTimeNew[1] > keyDelay){
        newIndex = findNext3x2(currentIndex, 1, 0);
        lastPressTimeNew[1] = now;
    }
    if(keyDown == HIGH && now - lastPressTimeNew[2] > keyDelay){
        newIndex = findNext3x2(currentIndex, 0, 1);
        lastPressTimeNew[2] = now;
    }
    if(keyUp == HIGH && now - lastPressTimeNew[3] > keyDelay){
        newIndex = findNext3x2(currentIndex, 0, -1);
        lastPressTimeNew[3] = now;
    }

    // เช็คว่ามีการเปลี่ยนปุ่มหรือไม่
    if (newIndex != currentIndex) {
        previousIndex = currentIndex;
        currentIndex = newIndex;
        changedFlag = true; // *** ตั้งค่า changedFlag เป็น true เมื่อมีการเปลี่ยนปุ่ม ***

        // Clear State เก่า
        lv_obj_clear_state(objects.bm_label1, LV_STATE_PRESSED);
        lv_obj_clear_state(objects.bm_label2, LV_STATE_PRESSED);
        lv_obj_clear_state(objects.bm_label3, LV_STATE_PRESSED);
        lv_obj_clear_state(objects.bm_label4, LV_STATE_PRESSED);
        lv_obj_clear_state(objects.bm_label5, LV_STATE_PRESSED);
        lv_obj_clear_state(objects.bm_label6, LV_STATE_PRESSED);

        // Add State ใหม่
        switch(currentIndex){
            case 0: lv_obj_add_state(objects.bm_label1, LV_STATE_PRESSED); break;
            case 1: lv_obj_add_state(objects.bm_label2, LV_STATE_PRESSED); break;
            case 2: lv_obj_add_state(objects.bm_label3, LV_STATE_PRESSED); break;
            case 3: lv_obj_add_state(objects.bm_label4, LV_STATE_PRESSED); break;
            case 4: lv_obj_add_state(objects.bm_label5, LV_STATE_PRESSED); break;
            case 5: lv_obj_add_state(objects.bm_label6, LV_STATE_PRESSED); break;
        }
    }
}
int currentIndex2 = 0;

void updatePgSwitch2x2() {
    // 1. อ่านค่าปุ่ม
    int keyLeft  = digitalRead(bt_3);
    int keyRight = digitalRead(bt_4);
    int keyDown  = digitalRead(bt_7);
    int keyUp    = digitalRead(bt_6);
    int keyEnt   = digitalRead(bt_9); 
    unsigned long now = millis();

    // 2. ป้องกันการกดเบิ้ล (Wait for Release)
    if (wait_for_release) {
        if (keyEnt == LOW) { 
            wait_for_release = false; 
            delay(50);
        } else {
            return; 
        }
    }

    // 3. เช็ค Dropdown ว่าเปิดอยู่ไหม (ถ้าเปิด ให้ปุ่มลูกศรไปคุม Dropdown แทน)
    
    // [Index 0] Hour Start
    if(lv_dropdown_is_open(objects.hour_setting)) {
        controlDropdown(objects.hour_setting, keyUp, keyDown);
        if(keyEnt == HIGH){ lv_dropdown_close(objects.hour_setting); delay(200); }
        return; 
    }
    // [Index 1] Min Start
    if(lv_dropdown_is_open(objects.min_setting)) {
        controlDropdown(objects.min_setting, keyUp, keyDown);
        if(keyEnt == HIGH){ lv_dropdown_close(objects.min_setting); delay(200); }
        return;
    }
    // [Index 2] Hour End (*** ต้องสร้าง Object นี้ใน UI ***)
    if(lv_dropdown_is_open(objects.hour_end)) {
        controlDropdown(objects.hour_end, keyUp, keyDown);
        if(keyEnt == HIGH){ lv_dropdown_close(objects.hour_end); delay(200); }
        return;
    }
    // [Index 3] Min End (*** ต้องสร้าง Object นี้ใน UI ***)
    if(lv_dropdown_is_open(objects.min_end)) {
        controlDropdown(objects.min_end, keyUp, keyDown);
        if(keyEnt == HIGH){ lv_dropdown_close(objects.min_end); delay(200); }
        return;
    }
    // [Index 4] Device
    if(lv_dropdown_is_open(objects.device_settting)) {
        controlDropdown(objects.device_settting, keyUp, keyDown);
        if(keyEnt == HIGH){ lv_dropdown_close(objects.device_settting); delay(200); }
        return;
    }

    // 4. การเลื่อนปุ่ม (Navigation)
    int newIndex = currentIndex2;
    if(keyLeft == HIGH && now - lastPressTimeNew[0] > keyDelay){
        newIndex = findNextSettings(currentIndex2, -1, 0);
        lastPressTimeNew[0] = now;
    }
    if(keyRight == HIGH && now - lastPressTimeNew[1] > keyDelay){
        newIndex = findNextSettings(currentIndex2, 1, 0);
        lastPressTimeNew[1] = now;
    }
    if(keyDown == HIGH && now - lastPressTimeNew[2] > keyDelay){
        newIndex = findNextSettings(currentIndex2, 0, 1);
        lastPressTimeNew[2] = now;
    }
    if(keyUp == HIGH && now - lastPressTimeNew[3] > keyDelay){
        newIndex = findNextSettings(currentIndex2, 0, -1);
        lastPressTimeNew[3] = now;
    }

    // 5. อัปเดต State การกด (Highlight)
    if (newIndex != currentIndex2) {
        // Clear old state
        lv_obj_clear_state(objects.hour_setting, LV_STATE_PRESSED);
        lv_obj_clear_state(objects.min_setting, LV_STATE_PRESSED);
        lv_obj_clear_state(objects.hour_end, LV_STATE_PRESSED); // New
        lv_obj_clear_state(objects.min_end, LV_STATE_PRESSED);  // New
        lv_obj_clear_state(objects.device_settting, LV_STATE_PRESSED);
        lv_obj_clear_state(objects.bt_save, LV_STATE_PRESSED);
        
        currentIndex2 = newIndex;
        
        // Set new state
        switch(currentIndex2){
            case 0: lv_obj_add_state(objects.hour_setting, LV_STATE_PRESSED); break;
            case 1: lv_obj_add_state(objects.min_setting, LV_STATE_PRESSED); break;
            case 2: lv_obj_add_state(objects.hour_end, LV_STATE_PRESSED); break; // New
            case 3: lv_obj_add_state(objects.min_end, LV_STATE_PRESSED); break;  // New
            case 4: lv_obj_add_state(objects.device_settting, LV_STATE_PRESSED); break;
            case 5: lv_obj_add_state(objects.bt_save, LV_STATE_PRESSED); break;
        }
    }

    // 6. การกด Enter (เปิด Dropdown หรือ บันทึก)
    if(keyEnt == HIGH) {
        switch(currentIndex2) {
            case 0: toggleDropdown(objects.hour_setting); break;
            case 1: toggleDropdown(objects.min_setting); break;
            case 2: toggleDropdown(objects.hour_end); break; // New
            case 3: toggleDropdown(objects.min_end); break;  // New
            case 4: toggleDropdown(objects.device_settting); break;
            case 5: // --- ปุ่ม Save ---
            {   
                char tempBuf[32]; 
                ProgramData p = loadProgram(curIndex); 
                
                // 1. Start Time
                lv_dropdown_get_selected_str(objects.hour_setting, tempBuf, sizeof(tempBuf));
                p.hour_start[edit_slot_id] = atoi(tempBuf); 
                
                lv_dropdown_get_selected_str(objects.min_setting, tempBuf, sizeof(tempBuf));
                p.minute_start[edit_slot_id] = atoi(tempBuf); 

                // 2. End Time (เพิ่มใหม่)
                lv_dropdown_get_selected_str(objects.hour_end, tempBuf, sizeof(tempBuf));
                p.hour_end[edit_slot_id] = atoi(tempBuf); 
                
                lv_dropdown_get_selected_str(objects.min_end, tempBuf, sizeof(tempBuf));
                p.minute_end[edit_slot_id] = atoi(tempBuf); 

                // 3. Device
                p.device[edit_slot_id] = lv_dropdown_get_selected(objects.device_settting); 

                saveProgram(curIndex, p);
                Serial.println("Saved All Data (Start/End) Correctly!");

                lv_scr_load_anim(objects.pg_switch, LV_SCR_LOAD_ANIM_FADE_OUT, 200, 0, false);
                needRefresh = true; 
            }
            break;
        }
        delay(200);
    }
}
void toggleDropdown(lv_obj_t* dd) {
    if(lv_dropdown_is_open(dd)) {
        lv_dropdown_close(dd);     // ปิด
    } else {
        lv_dropdown_open(dd);      // เปิด
    }
}
int controlDropdown(lv_obj_t *dd, int keyUp, int keyDown) {
    int cur = lv_dropdown_get_selected(dd);

    if(keyUp) {
        cur--;
        if(cur < 0) cur = 0;
        lv_dropdown_set_selected(dd, cur);
        delay(200);
    }

    if(keyDown) {
        cur++;
        int max = lv_dropdown_get_option_cnt(dd) - 1;
        if(cur > max) cur = max;
        lv_dropdown_set_selected(dd, cur);
        delay(200);
    }

    return cur;  
}



//----------setting-----------------------------
const unsigned long longPressTime = 1000;

bool lastState = false;
unsigned long pressStart1 = 0;
bool longPressTriggered = false;
void checkButton(int x, int y) {
    bool state = (btn_ent == HIGH);

    // เริ่มกดครั้งแรก
    if(state && !lastState){
        pressStart1 = millis();
        longPressTriggered = false;   // รีเซ็ตสถานะ
    }

    // กำลังกด และยังไม่เคยเรียก long-press
    if(state && !longPressTriggered){
        unsigned long dt = millis() - pressStart1;

        if(dt >= longPressTime){
            onLongPress(x, y);        // ทำงานทันที ไม่ต้องรอปล่อย
            longPressTriggered = true;
        }
    }

    // ปล่อยปุ่ม
    if(!state && lastState){
        if(!longPressTriggered){
            onShortPress(x);          // ถ้าไม่ใช่ long-press = short-press
        }
    }

    lastState = state;
}


void onShortPress(int x) {
    lv_obj_t *targets[6] = {
        objects.state_swich1,
        objects.state_swich2,
        objects.state_swich3,
        objects.state_swich4,
        objects.state_swich5,
        objects.state_swich6
    };

    if (x < 0 || x > 5) return;

    lv_obj_t *obj = targets[x];

    if (lv_obj_has_state(obj, LV_STATE_CHECKED))
        lv_obj_clear_state(obj, LV_STATE_CHECKED);
    else
        lv_obj_add_state(obj, LV_STATE_CHECKED);
}

void onLongPress(int x, int y) {
    edit_slot_id = x; 
    ProgramData p = loadProgram(y);

    // Set Start Time
    lv_dropdown_set_selected(objects.hour_setting, p.hour_start[edit_slot_id]);
    lv_dropdown_set_selected(objects.min_setting, p.minute_start[edit_slot_id]);
    
    // Set End Time (เพิ่มใหม่)
    lv_dropdown_set_selected(objects.hour_end, p.hour_end[edit_slot_id]);
    lv_dropdown_set_selected(objects.min_end, p.minute_end[edit_slot_id]);

    // Set Device
    lv_dropdown_set_selected(objects.device_settting, p.device[edit_slot_id]);

    currentIndex2 = 0;       
    wait_for_release = true; 

    lv_scr_load_anim(objects.set_time, LV_SCR_LOAD_ANIM_MOVE_LEFT, 200, 0, false);
}


/* ---------------- UPDATE BUTTON HIGHLIGHT ---------------- */
void updateButtonSelection() {
    lv_obj_t* currentScreen = lv_scr_act();
    if(currentScreen != objects.pg_work) return;

    int btnLeft  = digitalRead(bt_3);
    int btnRight = digitalRead(bt_4);
    int btnDown  = digitalRead(bt_7);
    int btnUp    = digitalRead(bt_6);
    unsigned long now = millis();

    int newIndex = curIndex;

    // ------------------ navigation ------------------
    if(btnLeft == HIGH && now - lastPressTime[0] > debounceDelay){
        newIndex = findNext(curIndex, -1, 0);
        lastPressTime[0] = now;
    }
    if(btnRight == HIGH && now - lastPressTime[1] > debounceDelay){
        newIndex = findNext(curIndex, 1, 0);
        lastPressTime[1] = now;
    }
    if(btnDown == HIGH && now - lastPressTime[2] > debounceDelay){
        newIndex = findNext(curIndex, 0, 1);
        lastPressTime[2] = now;
    }
    if(btnUp == HIGH && now - lastPressTime[3] > debounceDelay){
        newIndex = findNext(curIndex, 0, -1);
        lastPressTime[3] = now;
    }

    curIndex = newIndex;
    if(curIndex == 0) {lv_obj_add_state(objects.bm_0, LV_STATE_PRESSED);}
    else{lv_obj_clear_state(objects.bm_0, LV_STATE_PRESSED);}
    if(curIndex == 1) {lv_obj_add_state(objects.bm_1, LV_STATE_PRESSED);}
    else{lv_obj_clear_state(objects.bm_1, LV_STATE_PRESSED);}
    if(curIndex == 2) {lv_obj_add_state(objects.bm_2, LV_STATE_PRESSED);}
    else{lv_obj_clear_state(objects.bm_2, LV_STATE_PRESSED);}
    if(curIndex == 3) {lv_obj_add_state(objects.bm_3, LV_STATE_PRESSED);}
    else{lv_obj_clear_state(objects.bm_3, LV_STATE_PRESSED);}
    if(curIndex == 4) {lv_obj_add_state(objects.bm_4, LV_STATE_PRESSED);}
    else{lv_obj_clear_state(objects.bm_4, LV_STATE_PRESSED);}
    if(curIndex == 5) {lv_obj_add_state(objects.bm_5, LV_STATE_PRESSED);}
    else{lv_obj_clear_state(objects.bm_5, LV_STATE_PRESSED);}
    if(curIndex == 6) {lv_obj_add_state(objects.bm_6, LV_STATE_PRESSED);}
    else{lv_obj_clear_state(objects.bm_6, LV_STATE_PRESSED);}

    // ------------------ Enter เปลี่ยนหน้า ------------------
    if(btn_ent == HIGH){
        if(curIndex == 0) Day_pg = "Sunday";
        if(curIndex == 1) Day_pg = "Monday";
        if(curIndex == 2) Day_pg = "Tuesday";
        if(curIndex == 3) Day_pg = "Wednesday";
        if(curIndex == 4) Day_pg = "Thursday";
        if(curIndex == 5) Day_pg = "Friday";
        if(curIndex == 6) Day_pg = "Saturday";
        delay(300);
        lv_scr_load_anim(objects.pg_switch, LV_SCR_LOAD_ANIM_FADE_ON, 200, 0, false);
        
    }
}


//------------------divce------------------
const char* system(int d) {
  const char* days[] = { "NONE", "FAN", "LIGHT", "PUMP", "SPRAY" };
  return days[d];
}
// ประกาศตัวแปร Static ไว้นอกฟังก์ชันเพื่อจำค่าไว้ตลอด (ไม่หายเมื่อจบลูป)
static lv_obj_t * last_act_scr = NULL;
static unsigned long lastUpdate2 = 0; 

void CT_mode() {
    lv_obj_t* currentScreen = lv_scr_act();
    unsigned long now = millis();

    // -----------------------------------------------------------
    // 1. ตรวจสอบการเปลี่ยนหน้า (Screen Transition Check)
    // -----------------------------------------------------------
    // ถ้าเพิ่งเข้ามาหน้า pg_switch ให้บังคับโหลดข้อมูลใหม่ทันที
    if (currentScreen == objects.pg_switch && last_act_scr != objects.pg_switch) {
        needRefresh = true; 
    }
    last_act_scr = currentScreen; // จำหน้าปัจจุบันไว้เทียบรอบถัดไป

    // -----------------------------------------------------------
    // 2. Logic การเข้าสู่หน้า pg_work (หน้าเลือกวัน)
    // -----------------------------------------------------------
    // กดปุ่ม Home (bt_1) เพื่อเข้าหน้า pg_work
    if (digitalRead(bt_1) == HIGH && !pgWorkLoaded) {
        lv_scr_load_anim(objects.pg_work, LV_SCR_LOAD_ANIM_MOVE_TOP, 200, 0, false);
        pgWorkLoaded = true;
        delay(200); // ใส่ Delay เล็กน้อยป้องกันปุ่มเบิ้ลตอนเปลี่ยนหน้า
    } else if (currentScreen != objects.pg_work) {
        pgWorkLoaded = false;
    }

    // -----------------------------------------------------------
    // 3. Logic หน้า pg_work (หน้าเลือกวันจันทร์-อาทิตย์)
    // -----------------------------------------------------------
    if (currentScreen == objects.pg_work) {
        // *** แก้ไข: ใช้ static lastUpdate2 ทำให้เวลาหน่วงทำงานได้จริง ***
        if (now - lastUpdate2 >= 200) { 
            updateButtonSelection(); // ฟังก์ชันเลื่อนปุ่มเลือกวัน
            lastUpdate2 = now;
        }
        
        // กด ESC กลับหน้า Main
        if (escPressedOnce()) {
            if (btn_esc == HIGH) {
                lv_scr_load_anim(objects.main, LV_SCR_LOAD_ANIM_FADE_OUT, 200, 0, false);
            }
        }
    }

    // -----------------------------------------------------------
    // 4. Logic หน้า pg_switch (หน้าแสดงรายการ 6 ช่อง)
    // -----------------------------------------------------------
    if (currentScreen == objects.pg_switch) {
        updatePgSwitch(); // เช็คปุ่มเลื่อนขึ้นลง และอัปเดต changedFlag

        // โหลดข้อมูลเมื่อ: เพิ่งเข้าหน้า (needRefresh) หรือ มีการเลื่อนปุ่ม (changedFlag)
        if (needRefresh || changedFlag) { 
            ProgramData x = loadProgram(curIndex); // โหลดข้อมูลของวันที่เลือก
            updateAllLabels(x);    // อัปเดตตัวหนังสือ (เวลา/อุปกรณ์)
            updateAllSwitches(x);  // *** อัปเดตสถานะสวิตช์เปิด/ปิด ***
            
            needRefresh = false;   // รีเซ็ต flag
            changedFlag = false;   // รีเซ็ต flag
        }
        
        checkButton(currentIndex, curIndex); // เช็ค Short Press / Long Press
        
        // กด ESC กลับหน้าเลือกวัน (pg_work)
        if (escPressedOnce()) {
             if (btn_esc == HIGH) {
                 lv_scr_load_anim(objects.pg_work, LV_SCR_LOAD_ANIM_FADE_OUT, 200, 0, false); 
                 currentIndex = 0;
                 needRefresh = true; 
             }
        }
    }

    // -----------------------------------------------------------
    // 5. Logic หน้า set_time (หน้าตั้งค่าเวลา)
    // -----------------------------------------------------------
    if (currentScreen == objects.set_time) {
        // แสดงชื่อวัน และเลขช่อง (Slot ID)
        lv_label_set_text(objects.pg_switch_day_1, dayOfWeek(curIndex).c_str());
        lv_label_set_text(objects.num_save, String(edit_slot_id).c_str()); // ใช้ edit_slot_id จะถูกต้องกว่า currentIndex

        updatePgSwitch2x2(); // ฟังก์ชันคุมปุ่มในหน้าตั้งค่า
                
        // กด ESC ยกเลิกการตั้งค่า กลับไปหน้า pg_switch
        if (escPressedOnce()) {
            if (btn_esc == HIGH) {
                lv_scr_load_anim(objects.pg_switch, LV_SCR_LOAD_ANIM_FADE_OUT, 200, 0, false); 
                needRefresh = true; // บังคับโหลดข้อมูลใหม่เมื่อกลับไป
            }
        }
    }

    // -----------------------------------------------------------
    // 6. Logic หน้า Main และ Direac (Manual Control)
    // -----------------------------------------------------------
    // เข้าหน้า Direac
    if (btn_time == HIGH && currentScreen == objects.main) {
        lv_scr_load_anim(objects.direac, LV_SCR_LOAD_ANIM_MOVE_LEFT, 200, 0, false);
    }
    
    // ควบคุมในหน้า Direac
    if (currentScreen == objects.direac) {
        toggle_obj_state(objects.fan, stat3);
        toggle_obj_state(objects.light, stat4);
        toggle_obj_state(objects.pump, stat6);
        toggle_obj_state(objects.spray, stat7);
        
        if (stat5 == 1) {
            lv_obj_add_state(objects.mode_fan, LV_STATE_CHECKED);
            lv_label_set_text(objects.mode_fan_label, "HIGH");
        } else if (stat5 > 1) {
            lv_obj_clear_state(objects.mode_fan, LV_STATE_CHECKED);
            lv_label_set_text(objects.mode_fan_label, "LOW");
            stat5 = 0; 
        }
        
        // เช็คการส่ง Serial สำหรับ Fan Speed
        static bool prev_mode_fan = false;
        bool curr_mode_fan = lv_obj_has_state(objects.mode_fan, LV_STATE_CHECKED); 
        if (curr_mode_fan != prev_mode_fan) { 
            if (curr_mode_fan) Serial2.println("FAN_HIGH"); 
            else Serial2.println("FAN_LOW");  
            prev_mode_fan = curr_mode_fan; 
        }

        // กด ESC กลับหน้า Main
        if (btn_esc == HIGH) {
            lv_scr_load_anim(objects.main, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 200, 0, false);
        }
    }

    // -----------------------------------------------------------
    // 7. Background Tasks (ทำงานตลอดเวลา)
    // -----------------------------------------------------------
    // Sync สถานะปุ่มกับ UI และส่ง Serial
    sync_state(objects.fan, objects.fan_1, LV_STATE_CHECKED);
    sync_state(objects.light, objects.light_1, LV_STATE_CHECKED);
    sync_state(objects.pump, objects.pump_1, LV_STATE_CHECKED);
    sync_state(objects.spray, objects.spray_1, LV_STATE_CHECKED);

    // เช็คปุ่มเปลี่ยนโหมด Sensor (MS/FT)
    checkModeSensor();
}




void setup()
{
    // --- 1. Init Hardware ---
    pinMode(bt_1, INPUT);
    pinMode(bt_2, INPUT);
    pinMode(bt_3, INPUT);
    pinMode(bt_4, INPUT);
    pinMode(bt_5, INPUT);
    pinMode(bt_6, INPUT);
    pinMode(bt_7, INPUT);
    pinMode(bt_8, INPUT);
    pinMode(bt_9, INPUT);
    pinMode(bt_10, INPUT);
    pinMode(OutPin, INPUT);
    pinMode(led1, OUTPUT);
    pinMode(led2, OUTPUT);

    Serial.begin(115200);
    Serial2.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
    Serial2.setTimeout(10); // *** เพิ่ม: ลดการรอ Serial เพื่อให้ loop ไวขึ้น ***
    Serial2.println("Mute: OPEN"); 
    
    if (!rtc.begin()) {
        Serial.println("RTC NOT FOUND!");
    }

    initMemory();

    // --- 2. Init Display ---
    tft.begin();
    tft.setRotation(3); 

    // --- 3. Init LVGL ---
    lv_init();
    static lv_disp_draw_buf_t draw_buf;
    static lv_color_t buf[ SCREEN_WIDTH * 10 ]; 
    lv_disp_draw_buf_init( &draw_buf, buf, NULL, SCREEN_WIDTH * 10 );

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init( &disp_drv );
    disp_drv.hor_res = SCREEN_WIDTH;
    disp_drv.ver_res = SCREEN_HEIGHT;
    disp_drv.flush_cb = my_disp_flush; 
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register( &disp_drv );

    ui_init();
    
    // *** ลบ lv_timer_create ออก เพราะเราเรียกใน loop แล้ว ***
    Serial.println("Setup Done");
}

void loop()
{ 



  while (Serial2.available()) {
    String incomingData = Serial2.readStringUntil('\n'); 
    Serial.print("Received from Nano: ");
    Serial.println(incomingData);
  }
   handle_mute_button();
    bt();
    readWind();
    timer();
    CT_mode();
  lv_timer_handler(); 
  lv_tick_inc(5); // บอก LVGL ว่าเวลาผ่านไป 5ms (เท่ากับ delay ด้านล่าง)


}

