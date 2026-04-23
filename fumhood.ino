#include <Arduino.h>
#include <Wire.h>
#include <RTClib.h>
#include <lvgl.h>
#define M5GFX_USING_REAL_LVGL
#define LGFX_USE_V1 1
#include "LGFX_ILI9488_S3.hpp"
#include "ui.h" 
#include "memory.h"
#include "esp_system.h"
#include "funtion_control.h"

extern LGFX tft;
// ï¿½ï¿½Ë¹ï¿½ï¿½ï¿½Ò´Ë¹ï¿½Ò¨Íµï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ø³ï¿½ï¿½ï¿½ï¿½Ò¹ï¿½ï¿½Ô§
#define SCREEN_WIDTH  480 
#define SCREEN_HEIGHT 320

// ï¿½Ó¹Ç³ï¿½ï¿½Ò´ Buffer (ï¿½Í§ï¿½ï¿½ï¿½ 1/10 ï¿½Í§Ë¹ï¿½Ò¨ï¿½ï¿½ï¿½ç¹¤ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½é¹·ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½)
const uint32_t buffer_size = (SCREEN_WIDTH * SCREEN_HEIGHT / 10);

extern const lv_font_t ui_font_thai_14;
extern const lv_font_t ui_font_thai_16;
extern const lv_font_t ui_font_thai_20;
extern const lv_font_t ui_font_thai_22;
extern const lv_font_t ui_font_thai_30;
extern const lv_font_t ui_font_thai_18;
void initMemory();
#define RX_PIN 37 
#define TX_PIN 36

// --- Screensaver forward declarations ---
#define SS_TIMEOUT_MS  (5UL * 60UL * 1000UL)
extern unsigned long lastActivityTime;
extern bool ss_active;
void screensaver_exit();
void screensaver_enter();

void my_disp_flush( lv_display_t *disp, const lv_area_t *area, uint8_t *px_map )
{
    uint32_t w = ( area->x2 - area->x1 + 1 );
    uint32_t h = ( area->y2 - area->y1 + 1 );

    tft.startWrite();
    tft.setAddrWindow( area->x1, area->y1, w, h );
    tft.pushColors( ( uint16_t * )px_map, w * h, true );
    tft.endWrite();

    lv_display_flush_ready( disp );
}

static uint32_t my_tick(void)
{
    return millis();
}

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
#define bt_20 2
#define led_fan 35
#define led_light 45
#define led_auto 48
#define led_spray 42
#define led_pump 47

#include <ShiftRegister74HC595.h>
// --- ï¿½ï¿½Ð¡ï¿½È¢Òµï¿½Âµï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ---
const int SR_DATA_PIN  = 39;
const int SR_CLOCK_PIN = 40;
const int SR_LATCH_PIN = 41;

// ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Íµï¿½ï¿½ï¿½ï¿½ï¿½á·¹ï¿½ï¿½ï¿½ï¿½Å¢ã¹¤ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ò§ï¿½Ñµï¿½ï¿½
ShiftRegister74HC595<3> sr(SR_DATA_PIN, SR_CLOCK_PIN, SR_LATCH_PIN);
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
// ï¿½Ñ§ï¿½ï¿½ï¿½Ñ¹ï¿½ï¿½Å§ï¿½ï¿½ï¿½ï¿½Ñ¡ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ (ï¿½ï¿½Ñº Common Anode / Active Low)
uint8_t getCharPattern(char c) {
  // ï¿½ï¿½Å§ï¿½ï¿½ç¹¾ï¿½ï¿½ï¿½ï¿½ï¿½Ë­ï¿½
  if (c >= 'a' && c <= 'z') c -= 32; 

  switch (c) {
    // --- ï¿½ï¿½ï¿½ï¿½Å¢ (ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ø³ï¿½ï¿½ï¿½ï¿½ï¿½) ---
    case '0': return B11000000;
    case '1': return B11111001;
    case '2': return B10100100;
    case '3': return B10110000;
    case '4': return B10011001;
    case '5': return B10010010;
    case '6': return B10000010;
    case '7': return B11111000;
    case '8': return B10000000;
    case '9': return B10010000;

    // --- ï¿½ï¿½ï¿½ï¿½Ñ¡ï¿½ï¿½ (ï¿½Ó¹Ç³ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ñº Common Anode) ---
    case 'A': return B10001000; // A
    case 'B': return B10000011; // b
    case 'C': return B11000110; // C
    case 'D': return B10100001; // d
    case 'E': return B10000110; // E
    case 'F': return B10001110; // F
    case 'G': return B10000010; // G (ï¿½ï¿½ï¿½ï¿½Í¹ï¿½Å¢ 6)
    case 'H': return B10001001; // H
    case 'I': return B11111001; // I (ï¿½ï¿½ï¿½ï¿½Í¹ï¿½Å¢ 1)
    case 'J': return B11100001; // J
    case 'L': return B11000111; // L
    case 'N': return B10101011; // n
    case 'O': return B11000000; // O (ï¿½ï¿½ï¿½ï¿½Í¹ï¿½Å¢ 0)
    case 'P': return B10001100; // P
    case 'R': return B10101111; // r
    case 'S': return B10010010; // S (ï¿½ï¿½ï¿½ï¿½Í¹ï¿½Å¢ 5)
    case 'U': return B11000001; // U
    case 'Y': return B10010001; // y
    
    // --- ï¿½Ñ­ï¿½Ñ¡É³ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ---
    case '-': return B10111111; // ï¿½Õ´ï¿½ï¿½Ò§
    case ' ': return B11111111; // ï¿½Ñºä¿·Ø¡ï¿½Ç§
    default:  return B11111111; // ï¿½Ñºä¿¶ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ñ¡
  }
}
void textft(const char* text) {
  uint8_t seg1 = B11111111; // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½áººï¿½Ñºï¿½ï¿½ (Active Low)
  uint8_t seg2 = B11111111;
  uint8_t seg3 = B11111111;

  int len = strlen(text);
  if (len > 0) seg3 = getCharPattern(text[0]);
  if (len > 1) seg2 = getCharPattern(text[1]);
  if (len > 2) seg1 = getCharPattern(text[2]);

  uint8_t numberToPrint[] = { seg3, seg2, seg1 };
  sr.setAll(numberToPrint);
}
RTC_DS3231 rtc;
const int OutPin = 4;
float windms;
float windMS;
float windFtPerMin = 0;
bool changedFlag;
const int numSamples = 20;     
float windmsValues[numSamples];
int sampleIndex = 0;
bool filled = false;
extern float alert_set;
const char* mode = "auto";
const char* mode_senser = "ms";
bool silen = false;
int windsensor = 0;
const int led1 = 1;

const int relay_1 = 26;
const int relay_2 = 27;
int stat1, stat2, stat3, stat4, stat5, stat6, stat7, stat8, stat9, stat10;
int btn_home, btn_time, btn_light, btn_fan, btn_high, btn_pump, btn_spray, btn_esc, btn_ent, btn_mute;
char timeStr[10];
char ddmmyy[12];
char dayweek[12];
lv_obj_t* lastScreen = nullptr;
int currentDayIndex = 0; 
int currentSlotIndex = 0; 
bool needReloadPgSwitch = false;
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

    if(h_labels[idx] == NULL || d_labels[idx] == NULL) return; 

    char timeRange[32];
    sprintf(timeRange, "%02d:%02d\n%02d:%02d", 
            x.hour_start[idx], x.minute_start[idx], 
            x.hour_end[idx], x.minute_end[idx]);
    String dTxt = "";
    if(x.fan[idx])   dTxt += "F "; 
    if(!x.fan[idx])   dTxt += " - ";
    if(x.light[idx]) dTxt += "L "; 
    if(!x.light[idx]) dTxt += "- "; 
    if(x.pump[idx])  dTxt += "P "; 
    if(!x.pump[idx])  dTxt += "- "; 
    if(x.spray[idx]) dTxt += "S "; 
    if(!x.spray[idx]) dTxt += "- "; 
    if(x.reserve[idx]) dTxt += "R "; 
    if(!x.reserve[idx]) dTxt += "-"; 


    lv_label_set_text(h_labels[idx], timeRange);
    lv_label_set_text(d_labels[idx], dTxt.c_str());
}
void updateAllLabels(ProgramData &x) {
    for(int i=0; i<6; i++) {
        updateLabels(i, x);
    }
}
void digit(int senser) {

  int digit1 = senser / 100;
  int digit2 = (senser / 10) % 10;
  int digit3 = senser % 10;

  uint8_t seg3 = numberB[digit1] & B01111111; 
  uint8_t seg2 = numberB[digit2];
  uint8_t seg1 = numberB[digit3];

  uint8_t numberToPrint[] = { seg3, seg2, seg1 };
  sr.setAll(numberToPrint);
}

unsigned long previousMillisWind = 0;

char wind[32];
char windft[32];
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
float smoothedWindMs = 0;
float smoothedWindFt = 0;
//0.0069
float filterFactor = 0.006;
const unsigned long windInterval = 100; 
const unsigned long numplecs = 10000;
bool warring = true;
bool is_wind_zero_state = false; 
extern float rLow, rHigh;
extern float nLow, nHigh;
float windMPH;
void readWind() {
    static bool isInitialized = false; 
    pinMode(led1,OUTPUT);
    static float windFtPerMin = 0.0; 
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillisWind >= windInterval) {
        previousMillisWind = currentMillis;

        int windADunits = analogRead(OutPin);
 

        windMPH = pow((((float)windADunits - 264.0) / 85.6814), 3.36814);
        windMS = windMPH * 0.44704; 
        windms = mapFloat(windMS, rLow, rHigh, nLow, nHigh);
        // Serial2.print(rLow);
        // Serial2.print(" : ");
        // Serial2.print(rHigh);
        // Serial2.print(" : ");
        // Serial2.print(nLow);
        // Serial2.print(" : ");
        // Serial2.println(nHigh);
        // Serial.print(windMS);
        // Serial.print(" : ");
        // Serial.println(windADunits);
        if (windms < 0) windms = 0;

        windFtPerMin = windms * 196.8504; 

        windmsValues[sampleIndex] = windms;
        sampleIndex++;
        if (sampleIndex >= numSamples) {
            sampleIndex = 0;
            filled = true;
        }
    }
    smoothedWindMs = (smoothedWindMs * (1.0 - filterFactor)) + (windms * filterFactor);
    smoothedWindFt = (smoothedWindFt * (1.0 - filterFactor)) + (windFtPerMin * filterFactor);

    if (smoothedWindMs > alert_set ) {
        digitalWrite(led1, LOW);
        if (mode != "direct") {
            // Serial2.println("WIND_LOW");
        }
    }
        if (smoothedWindMs < alert_set ) {
        digitalWrite(led1, HIGH);
        if (mode != "direct") {
            // Serial2.println("WIND_HIGH");
        }
    }

    if (mode_senser == "ms") {
        int display_digit = smoothedWindMs * 100; 

        if (smoothedWindMs <= 0.01) { 
           textft("-L-");
            if(objects.senser) lv_label_set_text(objects.senser, "0.00 M/s");
            
        } 
        if (is_wind_zero_state == false && smoothedWindMs < alert_set) { 
             if (!lv_obj_has_state(objects.mode, LV_STATE_CHECKED)) {
                Serial2.println("WIND_LOW");
                Serial2.println("FAN_HIGH");
                lv_obj_add_state(objects.speed, LV_STATE_CHECKED);
                lv_obj_set_style_text_font(objects.m_fan, &ui_font_thai_18, 0);
                lv_label_set_text(objects.m_fan, "HIGH");
                if(objects.senser) lv_label_set_text(objects.senser, "0.00 M/s");
                textft("-L-");
                is_wind_zero_state = true;
             }
         }
        else {
            if (is_wind_zero_state == true  && smoothedWindMs >= alert_set) {
              if (!lv_obj_has_state(objects.mode, LV_STATE_CHECKED)) {
                Serial2.println("WIND_HIGH");
                Serial2.println("FAN_LOW");
                lv_obj_clear_state(objects.speed, LV_STATE_CHECKED);
                lv_obj_set_style_text_font(objects.m_fan, &ui_font_thai_18, 0);
                lv_label_set_text(objects.m_fan, "AUTO");

                is_wind_zero_state = false; 
              }
            }
            
            if (display_digit > 999) {
                textft("-H-");
                if(objects.senser) lv_label_set_text(objects.senser, "Over");
            } 
            if (display_digit < 999 && smoothedWindMs > alert_set){ 
                digit(display_digit); 
                char buf[32];
                snprintf(buf, sizeof(buf), "%.2f M/s", smoothedWindMs);
                
                if(objects.senser) lv_label_set_text(objects.senser, buf);
                // if(objects.senser_1) lv_label_set_text(objects.senser_1, buf);
            }
        }
    }
    if (mode_senser == "ft") {
        if (smoothedWindMs <= 0.01) {
            textft("-L-");
            if(objects.senser) lv_label_set_text(objects.senser, "0 FT/m");
        } 

        if (is_wind_zero_state == false && smoothedWindMs < alert_set) { 
                Serial2.println("WIND_LOW");
                if(objects.senser) lv_label_set_text(objects.senser, "0 FT/m");
                textft("-L-");
                is_wind_zero_state = true;
         }
        else {
            if (is_wind_zero_state == true  && smoothedWindMs >= alert_set) {
                Serial2.println("WIND_HIGH");
                delay(200);
                is_wind_zero_state = false; 
            }
            if (smoothedWindFt > 999) {
                textft("-H-");
                if(objects.senser) lv_label_set_text(objects.senser, "Over");
            } 
            if (smoothedWindFt < 999 && smoothedWindMs > alert_set){
                digitft((int)smoothedWindFt);
   
                char buf[32];
                snprintf(buf, sizeof(buf), "%.0f FT/m", smoothedWindFt);
                
                if(objects.senser) lv_label_set_text(objects.senser, buf);
            }
        }
    }
}

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
                // lv_obj_clear_state(objects.sound_2, LV_STATE_CHECKED);
                
                Serial2.println("Mute: OPEN"); 
            } else {
                warring = false;
                lv_obj_add_state(objects.sound_1, LV_STATE_CHECKED);
                // lv_obj_add_state(objects.sound_2, LV_STATE_CHECKED);
                
                Serial2.println("Mute: CLOSE"); 
            }
            lv_obj_invalidate(objects.sound_1);
            // lv_obj_invalidate(objects.sound_2);
        }
        delay(50);
    }
    lastBtnState = currentBtnState;
}
void digitft(int senser) {

  int digit1 = senser / 100;
  int digit2 = (senser / 10) % 10;
  int digit3 = senser % 10;

  uint8_t seg3 = numberB[digit1]; 
  uint8_t seg2 = numberB[digit2];
  uint8_t seg1 = numberB[digit3];

  uint8_t numberToPrint[] = { seg3, seg2, seg1 };
  sr.setAll(numberToPrint);
}

void timer() {
    static unsigned long lastTimerUpdate = 0;

    if (millis() - lastTimerUpdate < 1000) {
        return; 
    }
    lastTimerUpdate = millis();

    DateTime now = rtc.now(); 
    int dayOfWeekIndex = now.dayOfTheWeek();

    sprintf(timeStr, "%02d:%02d", now.hour(), now.minute());
    sprintf(ddmmyy, "%02d/%02d/%04d", now.day(), now.month(), now.year());
    strncpy(dayweek, dayOfWeek(dayOfWeekIndex).c_str(), sizeof(dayweek) - 1);
    dayweek[sizeof(dayweek) - 1] = '\0';
    // if(objects.timer) lv_label_set_text(objects.timer, timeStr);
    if(objects.timer1) lv_label_set_text(objects.timer1, timeStr);
    if(objects.dd_mm_yy_1) lv_label_set_text(objects.dd_mm_yy_1, ddmmyy);
    // if(objects.dd_mm_yy_2) lv_label_set_text(objects.dd_mm_yy_2, ddmmyy);
}

 int lastLoadedDay= -1;
void loadPgSwitchData() {
    ProgramData x = loadProgram(currentDayIndex);
    updateAllLabels(x);
    updateAllSwitches(x);
    lastLoadedDay = currentDayIndex;
}

void bt() {
  static unsigned long lastPress = 0;

  if (millis() - lastPress > 200) {
    lastPress = millis();
    btn_home  = digitalRead(bt_1);
    btn_time  = digitalRead(bt_2);
    btn_light = digitalRead(bt_3);
    btn_fan   = digitalRead(bt_4);
    btn_high  = digitalRead(bt_5);
    btn_pump  = digitalRead(bt_7);
    btn_spray = digitalRead(bt_6);
    btn_esc   = digitalRead(bt_8);
    btn_ent   = digitalRead(bt_9);
    btn_mute  = digitalRead(bt_10);

    bool anyBtn = (btn_home||btn_time||btn_light||btn_fan||btn_high||
                   btn_pump||btn_spray||btn_esc||btn_ent||btn_mute);
    if (anyBtn) {
        lastActivityTime = millis();
        if (ss_active) { screensaver_exit(); return; }  // wake only â€” don't process press
    }

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
}

void sync_state(lv_obj_t *src, lv_state_t state) {
    if(lv_obj_has_state(src, state)){
        lv_obj_add_state(src, state);
    }
    else{
        lv_obj_clear_state(src, state);
    }

    static bool prev_fan = false;
    static bool prev_light = false;
    static bool prev_pump = false;
    static bool prev_spray = false;
    static bool prev_reserve = false;

    static unsigned long fan_timer_start = 0;
    static bool fan_is_waiting = false;
    
    bool curr_fan = lv_obj_has_state(objects.fan, LV_STATE_CHECKED);
    if (curr_fan != prev_fan) {
        if (fan_is_waiting == false) {
            fan_timer_start = millis();
            fan_is_waiting = true;
        }
        if (millis() - fan_timer_start >= 5000) {
            if(curr_fan) {
                Serial2.println("FAN_ON");
            } else {
                Serial2.println("FAN_OFF");
            }
            prev_fan = curr_fan; 
            
            fan_is_waiting = false;
        }
    } else {
        fan_is_waiting = false;
    }

    bool curr_light = lv_obj_has_state(objects.light, LV_STATE_CHECKED);
    if (curr_light != prev_light) {
        if(curr_light) {
            Serial2.println("LIGHT_ON");
        } else {
            Serial2.println("LIGHT_OFF");
        }
        prev_light = curr_light;
    }

    bool curr_pump = lv_obj_has_state(objects.pump, LV_STATE_CHECKED);
    if (curr_pump != prev_pump) {
        if(curr_pump) {
            Serial2.println("PUMP_ON");
        } else {
            Serial2.println("PUMP_OFF");
        }
        prev_pump = curr_pump;
    }

    bool curr_spray = lv_obj_has_state(objects.spray, LV_STATE_CHECKED);
    if (curr_spray != prev_spray) {
        if(curr_spray) {
            Serial2.println("SPRAY_ON");
        } else {
            Serial2.println("SPRAY_OFF");
        }
        prev_spray = curr_spray;
    }

    bool curr_reserve = lv_obj_has_state(objects.reserve, LV_STATE_CHECKED);
    if (curr_reserve != prev_reserve) {
        if(curr_reserve) {
            Serial2.println("RESERVE_ON");
        } else {
            Serial2.println("RESERVE_OFF");
        }
        prev_reserve = curr_reserve;
    }

}
void simple_toggle(lv_obj_t *obj,lv_obj_t *pnl, int pin) {
   
    if (digitalRead(pin) == HIGH) {

        if (lv_obj_has_state(obj, LV_STATE_CHECKED)) {
            lv_obj_clear_state(obj, LV_STATE_CHECKED); 
             lv_obj_clear_state(pnl, LV_STATE_CHECKED); 
            if(obj == objects.speed){ lv_label_set_text(objects.m_fan, "LOW"); Serial2.println("FAN_LOW");}
        } else {
            lv_obj_add_state(obj, LV_STATE_CHECKED);
            lv_obj_add_state(pnl, LV_STATE_CHECKED);
            if(obj == objects.speed){ lv_label_set_text(objects.m_fan, "HIGH"); Serial2.println("FAN_HIGH");}
        }
        while (digitalRead(pin) == HIGH) {
            lv_timer_handler(); 
            delay(10);          
        }

        delay(300); 
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



int curIndex = 0;             
bool pgWorkLoaded = false;     
unsigned long lastPressTime[3] = {0,0,0};
const unsigned long debounceDelay = 200;

int findNext(int curIndex, int dx, int dy) {
    // á¼¹ï¿½Ñ§ï¿½ï¿½ï¿½ï¿½ (Map)
    const int buttonMap[3][3] = {
        {0, 1, 2},
        {3, 4, 5},
        {-1, 6, -1} 
    };
    
    int x = -1, y = -1;

    for(int row=0; row<3; row++){
        for(int col=0; col<3; col++){
            if(buttonMap[row][col] == curIndex) { 
                x = col; y = row; break; 
            }
        }
        if(x != -1) break;
    }
    

    if(x == -1) return curIndex;

    int nx = x;
    int ny = y;

    for(int i=0; i<9; i++){ 
        ny = (ny + dy + 3) % 3;
        nx = (nx + dx + 3) % 3; 

        if(buttonMap[ny][nx] != -1) {
            return buttonMap[ny][nx]; 
        }

    }

    return curIndex; 
}

int currentIndex = 0;            
int previousIndex = -1;     
int selectedButton = -1;       

int edit_slot_id = 0;
unsigned long lastPressTimeNew[4] = {0,0,0,0};
const unsigned long keyDelay = 200;
bool wait_for_release = false;

const int buttonMatrix[3][2] = {
    {0, 1},  // row 0
    {2, 3},  // row 1
    {4, 5}   // row 2
};

int findNext3x2(int curIndex, int dx, int dy) {

    int x = -1, y = -1;


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


    for(int i = 0; i < 6; i++){
        nx = (nx + dx + 2) % 2;  
        ny = (ny + dy + 3) % 3;

        if(buttonMatrix[ny][nx] != -1)
            return buttonMatrix[ny][nx];
    }

    return curIndex;
}

const int settingsMatrix[4][4] = {
    {0, 1, 0, 1},  // ï¿½ï¿½ï¿½ 0: Hour Start, Min Start (Ç¹ï¿½ï¿½ï¿½Â¢ï¿½ï¿½ï¿½ï¿½ï¿½)
    {2, 3, 2, 3},  // ï¿½ï¿½ï¿½ 1: Hour End, Min End (Ç¹ï¿½ï¿½ï¿½Â¢ï¿½ï¿½ï¿½ï¿½ï¿½)
    {4, 5, 9, 8},  // ï¿½ï¿½ï¿½ 2: Fan, Pump, Light, Spray
    {6, 7, 8, 6}   // ï¿½ï¿½ï¿½ 3: Save button (ï¿½ï¿½Íºï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ò§)
};

int findNextSettings(int curIdx, int dx, int dy) {
    int x = -1, y = -1;
    bool found = false;

    // ï¿½ï¿½ï¿½ï¿½Òµï¿½ï¿½Ë¹ï¿½ï¿½ (x, y) ï¿½Ñ¨ï¿½ØºÑ¹ï¿½Ò¡ Index
    for(int r = 0; r < 4 && !found; r++) {
        for(int c = 0; c < 4 && !found; c++) {
            if(settingsMatrix[r][c] == curIdx) { 
                y = r; 
                x = c; 
                found = true; 
            }
        }
    }
    
    // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ Index ï¿½Ñ¨ï¿½ØºÑ¹ ï¿½ï¿½ï¿½ï¿½×¹ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ñºï¿½ï¿½
    if(!found) return curIdx;

    // ï¿½Ó¹Ç³ï¿½ï¿½ï¿½Ë¹ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ (ï¿½Í§ï¿½Ñºï¿½ï¿½Ã¡ï¿½ï¿½ï¿½ï¿½Ø¢ÍºË¹ï¿½Ò¨ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ç¹ï¿½Ù»ï¿½ï¿½Ñºï¿½ï¿½ï¿½Õ¡ï¿½ï¿½ï¿½ï¿½)
    int nx = (x + dx + 4) % 4; // á¡¹ X ï¿½ï¿½ï¿½ï¿½ï¿½Â¹ï¿½ï¿½ï¿½ï¿½ +4 ï¿½ï¿½ï¿½ % 4
    int ny = (y + dy + 4) % 4; // á¡¹ Y ï¿½ï¿½ï¿½ï¿½ï¿½Â¹ï¿½ï¿½ï¿½ï¿½ +4 ï¿½ï¿½ï¿½ % 4

    return settingsMatrix[ny][nx];
}

const int timeMatrix[2][4] = {
    {0, 1, 2, 3},  
    {4, 5, 3, 2}   
};

int findNextTimeSetting(int curIdx, int dx, int dy) {
    int x = -1, y = -1;

    for(int r=0; r<2; r++){
        for(int c=0; c<4; c++){
            if(timeMatrix[r][c] == curIdx) { 
                y = r; 
                x = c; 
                
                goto found; 
            }
        }
    }
    found:
    if(x == -1) return curIdx; 

    int nx = (x + dx + 4) % 4; 
    int ny = (y + dy + 2) % 2; 


    int nextID = timeMatrix[ny][nx];

    if(nextID == curIdx && dx != 0) {
        if(dx > 0) nx = (nx + 1) % 4;   
        else       nx = (nx - 1 + 4) % 4; 
        nextID = timeMatrix[ny][nx];
    }

    return nextID;
}

int currentIndexCalib = 0; 
unsigned long lastPressTimeCalib[4] = {0, 0, 0, 0}; 

const int calibMatrix[2][3] = {
    {2, 0, 1}, 
    {2, 3, 4}  
};

int findNextCalibration(int curIdx, int dx, int dy) {
    int x = -1, y = -1;


    for(int r=0; r<2; r++){
        for(int c=0; c<3; c++){
            if(calibMatrix[r][c] == curIdx) { 
                y = r; 
                x = c; 
                goto found; 
            }
        }
    }
    found:
    if(x == -1) return curIdx; 

    int nx = (x + dx + 3) % 3; 
    int ny = (y + dy + 2) % 2; 

    return calibMatrix[ny][nx];
}

static bool isEditingWind = false;  
float tempWindValue = 0.0;  

static bool isEditingAlert = false;  
static float tempAlertValue = 0.0;   

static bool wait_for_release_calib = false; 

void updatePgCalibration() {
    int keyLeft  = digitalRead(bt_3);
    int keyRight = digitalRead(bt_4);
    int keyDown  = digitalRead(bt_7);
    int keyUp    = digitalRead(bt_6);
    int keyEnt   = digitalRead(bt_9); 
    unsigned long now = millis();
    if (wait_for_release_calib) {
        if (keyEnt == LOW) { 
            wait_for_release_calib = false; 
            delay(100); 
        } else {
            return; 
        }
    }

    if (isEditingWind || isEditingAlert) {
        
        bool valueChanged = false;
        if (isEditingWind) {
            if (keyUp == HIGH) {
                tempWindValue += 0.10;
                valueChanged = true;
            } else if (keyDown == HIGH) {
                tempWindValue -= 0.10;
                if (tempWindValue < 0) tempWindValue = 0;
                valueChanged = true;
            }
            
            if (valueChanged) {
                char buffer[10]; 
                char buffer1[10]; 
                sprintf(buffer,"%.2f", tempWindValue);
                sprintf(buffer1,"%.0f", tempWindValue * 196.85); 
                lv_label_set_text(objects.set_senser, buffer);
                lv_label_set_text(objects.set_senser_1, buffer1);
                delay(150);
            }
        }

        else if (isEditingAlert) {
            if (keyUp == HIGH) {
                tempAlertValue += 0.1;
                valueChanged = true;
            } else if (keyDown == HIGH) {
                tempAlertValue -= 0.1;
                if (tempAlertValue < 0) tempAlertValue = 0;
                valueChanged = true;
            }

            if (valueChanged) {
                char buffer[10]; 
                sprintf(buffer, "%.2f", tempAlertValue);
                lv_label_set_text(objects.set_alert, buffer);
                delay(150);
            }
        }

        if (keyEnt == HIGH) {
            if (isEditingWind) {
                isEditingWind = false;
                if(objects.lbl_wind_val) lv_obj_clear_state(objects.lbl_wind_val, LV_STATE_CHECKED);
            } 
            else if (isEditingAlert) {
                isEditingAlert = false;
                if(objects.lbl_alert_val) lv_obj_clear_state(objects.lbl_alert_val, LV_STATE_CHECKED);
            }
            
            wait_for_release_calib = true;
        }
        
        return; 
    }

    int newIndex = currentIndexCalib;
    const int navDelay = 250;

    if(keyLeft == HIGH && now - lastPressTimeCalib[0] > navDelay){
        newIndex = findNextCalibration(currentIndexCalib, -1, 0);
        lastPressTimeCalib[0] = now;
    }
    else if(keyRight == HIGH && now - lastPressTimeCalib[1] > navDelay){
        newIndex = findNextCalibration(currentIndexCalib, 1, 0);
        lastPressTimeCalib[1] = now;
    }
    else if(keyDown == HIGH && now - lastPressTimeCalib[2] > navDelay){
        newIndex = findNextCalibration(currentIndexCalib, 0, 1);
        lastPressTimeCalib[2] = now;
    }
    else if(keyUp == HIGH && now - lastPressTimeCalib[3] > navDelay){
        newIndex = findNextCalibration(currentIndexCalib, 0, -1);
        lastPressTimeCalib[3] = now;
    }

    if (newIndex != currentIndexCalib) {
        switch(currentIndexCalib){
            case 0: if(objects.lbl_wind_val) lv_obj_clear_state(objects.lbl_wind_val, LV_STATE_PRESSED); break;
            case 1: if(objects.lbl_alert_val) lv_obj_clear_state(objects.lbl_alert_val, LV_STATE_PRESSED); break;
            case 2: if(objects.btn_set_zero) lv_obj_clear_state(objects.btn_set_zero, LV_STATE_PRESSED); break;
            case 3: if(objects.btn_set_high) lv_obj_clear_state(objects.btn_set_high, LV_STATE_PRESSED); break;
            case 4: if(objects.btn_set_alert) lv_obj_clear_state(objects.btn_set_alert, LV_STATE_PRESSED); break;
        }

        currentIndexCalib = newIndex;
        switch(currentIndexCalib){
            case 0: if(objects.lbl_wind_val) lv_obj_add_state(objects.lbl_wind_val, LV_STATE_PRESSED); break;
            case 1: if(objects.lbl_alert_val) lv_obj_add_state(objects.lbl_alert_val, LV_STATE_PRESSED); break;
            case 2: if(objects.btn_set_zero) lv_obj_add_state(objects.btn_set_zero, LV_STATE_PRESSED); break;
            case 3: if(objects.btn_set_high) lv_obj_add_state(objects.btn_set_high, LV_STATE_PRESSED); break;
            case 4: if(objects.btn_set_alert) lv_obj_add_state(objects.btn_set_alert, LV_STATE_PRESSED); break;
        }
    }

    if(keyEnt == HIGH) {
        wait_for_release_calib = true;

        switch(currentIndexCalib) {
            case 0: 
                isEditingWind = true;
                if(objects.lbl_wind_val) lv_obj_add_state(objects.lbl_wind_val, LV_STATE_CHECKED);
                if(objects.set_senser) {
                    char buffer[10];
                    sprintf(buffer, "%.2f", tempWindValue); 
                    lv_label_set_text(objects.set_senser, buffer);
                }
                break;

            case 1: 
                isEditingAlert = true;
                if(objects.lbl_alert_val) lv_obj_add_state(objects.lbl_alert_val, LV_STATE_CHECKED);
                if(objects.set_alert) {
                    char buffer[10];
                    sprintf(buffer, "%.2f", tempAlertValue);
                    lv_label_set_text(objects.set_alert, buffer);
                }
                break;
            
            case 2: 
                if(objects.btn_set_zero) lv_obj_add_state(objects.btn_set_zero, LV_STATE_CHECKED);
                saveLow(windMS, 0.00); 
                delay(150);
                if(objects.btn_set_zero) lv_obj_clear_state(objects.btn_set_zero, LV_STATE_CHECKED);
                break;
                
            case 3: 
                if(objects.btn_set_high) lv_obj_add_state(objects.btn_set_high, LV_STATE_CHECKED);
                saveHigh(windMS, tempWindValue);
                delay(150);
                if(objects.btn_set_high) lv_obj_clear_state(objects.btn_set_high, LV_STATE_CHECKED);
                break;
                
            case 4: 
                if(objects.btn_set_alert) lv_obj_add_state(objects.btn_set_alert, LV_STATE_CHECKED);
                    saveAlertSetting(tempAlertValue);
                    alert_set = tempAlertValue; 
                    delay(150);
                    if(objects.btn_set_alert) lv_obj_clear_state(objects.btn_set_alert, LV_STATE_CHECKED);
                
                break;
        }
    }
}
String Day_pg;


void updateAllSwitches(ProgramData &x) {

    lv_obj_t* sw_targets[] = {
        objects.state_swich1, objects.state_swich2, objects.state_swich3,
        objects.state_swich4, objects.state_swich5, objects.state_swich6
    };

    for (int i = 0; i < 6; i++) {

        if(sw_targets[i] == NULL) continue; 

        if (x.state[i] == 1) {

            lv_obj_add_state(sw_targets[i], LV_STATE_CHECKED);
        } else {

            lv_obj_clear_state(sw_targets[i], LV_STATE_CHECKED);
        }

        lv_obj_invalidate(sw_targets[i]);
    }
}

void updatePgSwitch() {
    lv_label_set_text(objects.pg_switch_day, Day_pg.c_str());

    int keyLeft  = digitalRead(bt_3);
    int keyRight = digitalRead(bt_4);
    int keyDown  = digitalRead(bt_7);
    int keyUp    = digitalRead(bt_6);
    unsigned long now = millis();

    int newIndex = currentIndex;
    const int navDelay = 250; 

    if(keyLeft == HIGH && now - lastPressTimeNew[0] > navDelay){
        newIndex = findNext3x2(currentIndex, -1, 0);
        lastPressTimeNew[0] = now;
    }
    else if(keyRight == HIGH && now - lastPressTimeNew[1] > navDelay){
        newIndex = findNext3x2(currentIndex, 1, 0);
        lastPressTimeNew[1] = now;
    }
    else if(keyDown == HIGH && now - lastPressTimeNew[2] > navDelay){
        newIndex = findNext3x2(currentIndex, 0, 1);
        lastPressTimeNew[2] = now;
    }
    else if(keyUp == HIGH && now - lastPressTimeNew[3] > navDelay){
        newIndex = findNext3x2(currentIndex, 0, -1);
        lastPressTimeNew[3] = now;
    }

    if (newIndex != currentIndex) {
        previousIndex = currentIndex;
        currentIndex = newIndex;
        changedFlag = true; 

        lv_obj_clear_state(objects.bm_label1, LV_STATE_PRESSED);
        lv_obj_clear_state(objects.bm_label2, LV_STATE_PRESSED);
        lv_obj_clear_state(objects.bm_label3, LV_STATE_PRESSED);
        lv_obj_clear_state(objects.bm_label4, LV_STATE_PRESSED);
        lv_obj_clear_state(objects.bm_label5, LV_STATE_PRESSED);
        lv_obj_clear_state(objects.bm_label6, LV_STATE_PRESSED);

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
int getDropdownValue(lv_obj_t * obj) {
    if (obj == NULL || !lv_obj_is_valid(obj)) return 0;

    // ï¿½ï¿½Í§ï¿½ï¿½ (char*) ï¿½ï¿½Ë¹ï¿½ï¿½
    char * buf = (char*)lv_malloc(64); 
    
    if (buf == NULL) return 0;

    lv_memset(buf, 0, 64);
    lv_dropdown_get_selected_str(obj, buf, 64);
    
    int result = atoi(buf);

    lv_free(buf); // Free

    return result;
}
void toggleCheckState(lv_obj_t* obj) {
    if (lv_obj_has_state(obj, LV_STATE_CHECKED)) {
        lv_obj_clear_state(obj, LV_STATE_CHECKED);
    } else {
        lv_obj_add_state(obj, LV_STATE_CHECKED);
    }
}
void updatePgSwitch2x2() {
    int keyLeft  = digitalRead(bt_3);
    int keyRight = digitalRead(bt_4);
    int keyDown  = digitalRead(bt_7);
    int keyUp    = digitalRead(bt_6);
    int keyEnt   = digitalRead(bt_9); 
    unsigned long now = millis();

    if (wait_for_release) {
        if (keyEnt == LOW) { 
            wait_for_release = false; 
            delay(100); 
        } else {
            return; 
        }
    }

    lv_obj_t* dds[] = { objects.hour_setting, objects.min_setting, objects.hour_end, objects.min_end };
    
    for(int i = 0; i < 4; i++) {
        if(dds[i] != NULL && lv_dropdown_is_open(dds[i])) {
            controlDropdown(dds[i], keyUp, keyDown);
            if(keyEnt == HIGH) {
                lv_dropdown_close(dds[i]);
                wait_for_release = true; 
            }
            return; 
        }
    }

    lv_obj_clear_flag( objects.hour_setting, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(objects.min_setting, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(objects.hour_end, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(objects.min_end, LV_OBJ_FLAG_HIDDEN);

    int newIndex = currentIndex2;
    const int navDelay = 250;

    if(keyLeft == HIGH && now - lastPressTimeNew[0] > navDelay){
        newIndex = findNextSettings(currentIndex2, -1, 0);
        lastPressTimeNew[0] = now;
    }
    else if(keyRight == HIGH && now - lastPressTimeNew[1] > navDelay){
        newIndex = findNextSettings(currentIndex2, 1, 0);
        lastPressTimeNew[1] = now;
    }
    else if(keyDown == HIGH && now - lastPressTimeNew[2] > navDelay){
        newIndex = findNextSettings(currentIndex2, 0, 1);
        lastPressTimeNew[2] = now;
    }
    else if(keyUp == HIGH && now - lastPressTimeNew[3] > navDelay){
        newIndex = findNextSettings(currentIndex2, 0, -1);
        lastPressTimeNew[3] = now;
    }

    if (newIndex != currentIndex2) {
        
        if(objects.hour_setting) lv_obj_clear_state(objects.hour_setting, LV_STATE_PRESSED);
        if(objects.min_setting)  lv_obj_clear_state(objects.min_setting, LV_STATE_PRESSED);
        if(objects.hour_end)     lv_obj_clear_state(objects.hour_end, LV_STATE_PRESSED);
        if(objects.min_end)      lv_obj_clear_state(objects.min_end, LV_STATE_PRESSED);

        if(objects.fan_state)    lv_obj_clear_state(objects.fan_state, LV_STATE_PRESSED);
        if(objects.pump_state)   lv_obj_clear_state(objects.pump_state, LV_STATE_PRESSED);
        if(objects.light_state)  lv_obj_clear_state(objects.light_state, LV_STATE_PRESSED);
        if(objects.spray_state)  lv_obj_clear_state(objects.spray_state, LV_STATE_PRESSED);
        
        // --- ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½â¿¡ï¿½Ê¢Í§ reserve_state ---
        if(objects.reserve_state) lv_obj_clear_state(objects.reserve_state, LV_STATE_PRESSED);

        if(objects.bt_save)      lv_obj_clear_state(objects.bt_save, LV_STATE_PRESSED);

        currentIndex2 = newIndex;

        switch(currentIndex2){
            case 0: if(objects.hour_setting) lv_obj_add_state(objects.hour_setting, LV_STATE_PRESSED); break;
            case 1: if(objects.min_setting)  lv_obj_add_state(objects.min_setting, LV_STATE_PRESSED); break;
            case 2: if(objects.hour_end)     lv_obj_add_state(objects.hour_end, LV_STATE_PRESSED); break;
            case 3: if(objects.min_end)      lv_obj_add_state(objects.min_end, LV_STATE_PRESSED); break;
            
            case 4: if(objects.fan_state)    lv_obj_add_state(objects.fan_state, LV_STATE_PRESSED); break;
            case 5: if(objects.pump_state)   lv_obj_add_state(objects.pump_state, LV_STATE_PRESSED); break;
            case 6: if(objects.light_state)  lv_obj_add_state(objects.light_state, LV_STATE_PRESSED); break;
            case 7: if(objects.spray_state)  lv_obj_add_state(objects.spray_state, LV_STATE_PRESSED); break;
            
            // --- ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ò´â¿¡ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ñº reserve_state ---
            case 9: if(objects.reserve_state) lv_obj_add_state(objects.reserve_state, LV_STATE_PRESSED); break;
            
            case 8: if(objects.bt_save)      lv_obj_add_state(objects.bt_save, LV_STATE_PRESSED); break;
        }
    }

    if(keyEnt == HIGH) {
        wait_for_release = true; 
        
        switch(currentIndex2) {
            case 0: toggleDropdown(objects.hour_setting); break;
            case 1: toggleDropdown(objects.min_setting); break;
            case 2: toggleDropdown(objects.hour_end); break;
            case 3: toggleDropdown(objects.min_end); break;
            case 4: { 
                if(lv_obj_has_state(objects.fan_state, LV_STATE_CHECKED)) lv_obj_clear_state(objects.fan_state, LV_STATE_CHECKED);
                else lv_obj_add_state(objects.fan_state, LV_STATE_CHECKED);
                break;
            }
            case 5: {
                if(lv_obj_has_state(objects.pump_state, LV_STATE_CHECKED)) lv_obj_clear_state(objects.pump_state, LV_STATE_CHECKED);
                else lv_obj_add_state(objects.pump_state, LV_STATE_CHECKED);
                break;
            }
            case 6: {
                if(lv_obj_has_state(objects.light_state, LV_STATE_CHECKED)) lv_obj_clear_state(objects.light_state, LV_STATE_CHECKED);
                else lv_obj_add_state(objects.light_state, LV_STATE_CHECKED);
                break;
            }
            case 7: {
                if(lv_obj_has_state(objects.spray_state, LV_STATE_CHECKED)) lv_obj_clear_state(objects.spray_state, LV_STATE_CHECKED);
                else lv_obj_add_state(objects.spray_state, LV_STATE_CHECKED);
                break;
            }
            // --- ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ã·Ó§Ò¹ï¿½Í¹ï¿½ï¿½ Enter ï¿½ï¿½ï¿½ reserve_state ---
            case 9: {
                if(lv_obj_has_state(objects.reserve_state, LV_STATE_CHECKED)) lv_obj_clear_state(objects.reserve_state, LV_STATE_CHECKED);
                else lv_obj_add_state(objects.reserve_state, LV_STATE_CHECKED);
                break;
            }
            case 8:
            {
                ProgramData p = loadProgram(currentDayIndex);

                p.hour_start[edit_slot_id]   = getDropdownValue(objects.hour_setting);
                p.minute_start[edit_slot_id] = getDropdownValue(objects.min_setting);
                p.hour_end[edit_slot_id]     = getDropdownValue(objects.hour_end);
                p.minute_end[edit_slot_id]   = getDropdownValue(objects.min_end);

                p.fan[edit_slot_id]   = lv_obj_has_state(objects.fan_state, LV_STATE_CHECKED) ? 1 : 0;
                p.pump[edit_slot_id]  = lv_obj_has_state(objects.pump_state, LV_STATE_CHECKED) ? 1 : 0;
                p.light[edit_slot_id] = lv_obj_has_state(objects.light_state, LV_STATE_CHECKED) ? 1 : 0;
                p.spray[edit_slot_id] = lv_obj_has_state(objects.spray_state, LV_STATE_CHECKED) ? 1 : 0;
                p.reserve[edit_slot_id] = lv_obj_has_state(objects.reserve_state, LV_STATE_CHECKED) ? 1 : 0;

                p.state[edit_slot_id] = 1;

                saveSingleSlot(currentDayIndex, edit_slot_id, p);

                lv_scr_load_anim(objects.pg_switch, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
                needReloadPgSwitch = true; 
                lv_obj_add_flag(objects.hour_setting, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(objects.min_setting, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(objects.hour_end, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(objects.min_end, LV_OBJ_FLAG_HIDDEN);
            }
            break;
        }
    }
}

int currentIndexTime = 0; 

void updatePgTime() {
    int keyLeft  = digitalRead(bt_3);
    int keyRight = digitalRead(bt_4);
    int keyDown  = digitalRead(bt_7);
    int keyUp    = digitalRead(bt_6);
    int keyEnt   = digitalRead(bt_9); 
    unsigned long now = millis();

    if (wait_for_release) {
        if (keyEnt == LOW) wait_for_release = false;
        return; 
    }

    // ï¿½ï¿½Â¡ï¿½ï¿½ Dropdown ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ (ï¿½Ñ´ï¿½ï¿½ï¿½Â§ï¿½ï¿½ï¿½ index ï¿½ï¿½Ô§ã¹¨ï¿½)
    // 0:DD, 1:MM, 2:YY, (3:Save), 4:HH, 5:Min
    lv_obj_t* dds[] = { objects.set_dd, objects.set_mm, objects.set_yy, NULL, objects.set_hh, objects.set_min };

    // 1. ï¿½Ñ´ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ Dropdown ï¿½ï¿½Ô´ï¿½ï¿½ï¿½ï¿½
    if(currentIndexTime != 3) {
        lv_obj_t* currObj = dds[currentIndexTime];
        if(currObj != NULL && lv_dropdown_is_open(currObj)) {
            controlDropdown(currObj, keyUp, keyDown); 
            if(keyEnt == HIGH) { 
                lv_dropdown_close(currObj);
                wait_for_release = true;
            }
            return;
        }
    }

    // 2. ï¿½Ñ´ï¿½ï¿½Ã¡ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Í¹ï¿½ï¿½ï¿½Ë¹ï¿½ï¿½ (Navigation)
    int newIndex = currentIndexTime;
    const int navDelay = 250;
    if(keyLeft == HIGH && now - lastPressTimeNew[0] > navDelay){
        newIndex = findNextTimeSetting(currentIndexTime, -1, 0);
        lastPressTimeNew[0] = now;
    }
    else if(keyRight == HIGH && now - lastPressTimeNew[1] > navDelay){
        newIndex = findNextTimeSetting(currentIndexTime, 1, 0);
        lastPressTimeNew[1] = now;
    }
    else if(keyDown == HIGH && now - lastPressTimeNew[2] > navDelay){
        newIndex = findNextTimeSetting(currentIndexTime, 0, 1);
        lastPressTimeNew[2] = now;
    }
    else if(keyUp == HIGH && now - lastPressTimeNew[3] > navDelay){
        newIndex = findNextTimeSetting(currentIndexTime, 0, -1);
        lastPressTimeNew[3] = now;
    }

    if (newIndex != currentIndexTime) {
        // 1. ï¿½ï¿½Ò§Ê¶Ò¹ï¿½ Pressed ï¿½Í¡ï¿½Ò¡ï¿½Ø¡ Object ã¹¡ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Í¹
        if(objects.set_dd)   lv_obj_clear_state(objects.set_dd, LV_STATE_PRESSED);
        if(objects.set_mm)   lv_obj_clear_state(objects.set_mm, LV_STATE_PRESSED);
        if(objects.set_yy)   lv_obj_clear_state(objects.set_yy, LV_STATE_PRESSED);
        if(objects.set_save) lv_obj_clear_state(objects.set_save, LV_STATE_PRESSED);
        if(objects.set_hh)   lv_obj_clear_state(objects.set_hh, LV_STATE_PRESSED);
        if(objects.set_min)  lv_obj_clear_state(objects.set_min, LV_STATE_PRESSED);

        currentIndexTime = newIndex;

        // 2. ï¿½ï¿½ï¿½ï¿½ï¿½Ê¶Ò¹ï¿½ Pressed ï¿½ï¿½ï¿½ï¿½Ñº Object ï¿½ï¿½ï¿½ï¿½Ù¡ï¿½ï¿½ï¿½Í¡ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ Switch-Case
        switch(currentIndexTime) {
            case 0: if(objects.set_dd)   lv_obj_add_state(objects.set_dd, LV_STATE_PRESSED);   break;
            case 1: if(objects.set_mm)   lv_obj_add_state(objects.set_mm, LV_STATE_PRESSED);   break;
            case 2: if(objects.set_yy)   lv_obj_add_state(objects.set_yy, LV_STATE_PRESSED);   break;
            case 3: if(objects.set_save) lv_obj_add_state(objects.set_save, LV_STATE_PRESSED); break;
            case 4: if(objects.set_hh)   lv_obj_add_state(objects.set_hh, LV_STATE_PRESSED);   break;
            case 5: if(objects.set_min)  lv_obj_add_state(objects.set_min, LV_STATE_PRESSED);  break;
        }
    }
if(keyEnt == HIGH) {
        wait_for_release = true; 

        switch(currentIndexTime) {
            // --- ï¿½ï¿½ï¿½ï¿½ï¿½ Dropdown (ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ô´/ï¿½Ô´ï¿½ï¿½Êµï¿½) ---
            case 0: toggleDropdown(objects.set_dd);  break;
            case 1: toggleDropdown(objects.set_mm);  break;
            case 2: toggleDropdown(objects.set_yy);  break;
            case 4: toggleDropdown(objects.set_hh);  break;
            case 5: toggleDropdown(objects.set_min); break;

            // --- Case 3: ï¿½ï¿½ï¿½ï¿½ SAVE (ï¿½ï¿½Ë¹ï¿½Ò·ï¿½ï¿½ï¿½Çºï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ÐºÑ¹ï¿½Ö¡) ---
            case 3: 
            {
                Serial.println(F("[SYSTEM] Save Button Pressed"));

                // 1. ï¿½Ö§ï¿½ï¿½Ò¨Ò¡ Dropdown ï¿½ï¿½ï¿½ï¿½Ðµï¿½ï¿½ (ï¿½ï¿½ï¿½ getDropdownValue áºº Index ï¿½ï¿½ï¿½ï¿½ï¿½Ò·Ó¡Ñ¹ï¿½ï¿½ï¿½)
                int d  = getDropdownValue(objects.set_dd);
                int m  = getDropdownValue(objects.set_mm);
                int y  = getDropdownValue(objects.set_yy); // ï¿½ï¿½ (Mode 1)
                int h  = getDropdownValue(objects.set_hh); // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ (Mode 2)
                int mn = getDropdownValue(objects.set_min);

                    if(objects.set_dd)  lv_dropdown_close(objects.set_dd);
                    if(objects.set_mm)  lv_dropdown_close(objects.set_mm);
                    if(objects.set_yy)  lv_dropdown_close(objects.set_yy);
                    if(objects.set_hh)  lv_dropdown_close(objects.set_hh);
                    if(objects.set_min) lv_dropdown_close(objects.set_min);
                    
                    vTaskDelay(10); // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ UI ï¿½×¹ï¿½ï¿½ï¿½ Memory

                    // 3. ï¿½Ñ¹ï¿½Ö¡ï¿½ï¿½ï¿½Å§ Memory (Preferences) ï¿½ï¿½ï¿½ RTC
                    saveTimeSettings(d, m, y, h, mn);
                    
                    vTaskDelay(20); // Ë¹ï¿½Ç§ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ I2C Bus ï¿½ï¿½ï¿½ï¿½

                    // 4. ï¿½Ñ»à´µï¿½ï¿½ï¿½ï¿½Å¢ï¿½ï¿½ï¿½Ò·ï¿½ï¿½ï¿½Ê´ï¿½ï¿½ï¿½Ë¹ï¿½Ò¨Í·Ñ¹ï¿½ï¿½
                    DateTime rtcNow = rtc.now();
                    static char dBuf[32], tBuf[16];
                    
                    snprintf(dBuf, sizeof(dBuf), "%02d/%02d/%d", rtcNow.day(), rtcNow.month(), rtcNow.year());
                    snprintf(tBuf, sizeof(tBuf), "%02d:%02d", rtcNow.hour(), rtcNow.minute());
                    
                    if(objects.ddmmyy_1) lv_label_set_text(objects.ddmmyy_1, dBuf);
                    if(objects.timer_3)  lv_label_set_text(objects.timer_3, tBuf);
                    showProcessScreen("SYSTEM: SAVE", "Saving Config...", "SAVING", 2000, TFT_GREEN);
                    delay(2000);
                    showProcessScreen("SYSTEM: RESET", "Rebooting...", "RESET", 3000, TFT_RED);
                    delay(1000);
                    ESP.restart(); 
                    Serial2.println("restart");
                break;
            }
        }
    }
}
void toggleDropdown(lv_obj_t* dd) {
    if(lv_dropdown_is_open(dd)) {
        lv_dropdown_close(dd);   
    } else {
        lv_dropdown_open(dd);    
    }
}
int controlDropdown(lv_obj_t *dd, int keyUp, int keyDown) {
    int cur = lv_dropdown_get_selected(dd);

    if(keyUp) {
        cur--;
        if(cur < 0) cur = 0;
        lv_dropdown_set_selected(dd, cur);
        delay(150); 
    }

    if(keyDown) {
        cur++;
        int max = lv_dropdown_get_option_cnt(dd) - 1;
        if(cur > max) cur = max;
        lv_dropdown_set_selected(dd, cur);
        delay(150); 
    }
    return cur;  
}

const unsigned long longPressTime = 1000;
unsigned long lastBtnTime = 0;
const unsigned long debounceMs = 200;
bool lastState = false;
unsigned long pressStart1 = 0;
bool longPressTriggered = false;
void checkButton(int x, int y) {
    bool state = (btn_ent == HIGH);
    if(state && !lastState){
        pressStart1 = millis();
        longPressTriggered = false;
    }

    if(state && !longPressTriggered){
        unsigned long dt = millis() - pressStart1;

        if(dt >= longPressTime){
            onLongPress(x, y); 
            longPressTriggered = true;
        }
    }

   if (!state && lastState) {
    unsigned long now = millis();

    if (!longPressTriggered && (now - lastBtnTime > debounceMs)) {
        lastBtnTime = now;
        onShortPress(x);
    }
}


    lastState = state;
}
    lv_obj_t *targets[6] = {
        objects.state_swich1,
        objects.state_swich2,
        objects.state_swich3,
        objects.state_swich4,
        objects.state_swich5,
        objects.state_swich6
    };
volatile bool shortPressPending = false;
int shortPressSlot = -1;
void onShortPress(int slot) {
    if (shortPressPending) return; 

    shortPressSlot    = slot;
    shortPressPending = true;
}
void handleShortPressTask() {
    if (!shortPressPending) return;
    shortPressPending = false;

    lv_obj_t *sw_targets[6] = {
        objects.state_swich1, objects.state_swich2, objects.state_swich3,
        objects.state_swich4, objects.state_swich5, objects.state_swich6
    };

    if (shortPressSlot < 0 || shortPressSlot > 5) return;

    ProgramData p = loadProgram(currentDayIndex);
    p.state[shortPressSlot] = !p.state[shortPressSlot];
    saveSingleSlot(currentDayIndex, shortPressSlot, p);

    if (p.state[shortPressSlot]) {
        lv_obj_add_state(sw_targets[shortPressSlot], LV_STATE_CHECKED);
    } else {
        lv_obj_clear_state(sw_targets[shortPressSlot], LV_STATE_CHECKED);
    }
}
void onLongPress(int slot_index, int day_index) {
    edit_slot_id = slot_index; 
    currentDayIndex = day_index;

    if (edit_slot_id < 0 || edit_slot_id > 5) {
        return;
    }

    ProgramData p = loadProgram(currentDayIndex);

    if(objects.hour_setting != NULL) lv_dropdown_set_selected(objects.hour_setting, p.hour_start[edit_slot_id]);
    if(objects.min_setting != NULL)  lv_dropdown_set_selected(objects.min_setting, p.minute_start[edit_slot_id]);
    if(objects.hour_end != NULL)     lv_dropdown_set_selected(objects.hour_end, p.hour_end[edit_slot_id]);
    if(objects.min_end != NULL)      lv_dropdown_set_selected(objects.min_end, p.minute_end[edit_slot_id]);

    if (objects.fan_state != NULL) {
        if(p.fan[edit_slot_id] == 1) lv_obj_add_state(objects.fan_state, LV_STATE_CHECKED);
        else lv_obj_clear_state(objects.fan_state, LV_STATE_CHECKED);
    }

    if (objects.pump_state != NULL) {
        if(p.pump[edit_slot_id] == 1) lv_obj_add_state(objects.pump_state, LV_STATE_CHECKED);
        else lv_obj_clear_state(objects.pump_state, LV_STATE_CHECKED);
    }

    if (objects.light_state != NULL) {
        if(p.light[edit_slot_id] == 1) lv_obj_add_state(objects.light_state, LV_STATE_CHECKED);
        else lv_obj_clear_state(objects.light_state, LV_STATE_CHECKED);
    }

    if (objects.spray_state != NULL) {
        if(p.spray[edit_slot_id] == 1) lv_obj_add_state(objects.spray_state, LV_STATE_CHECKED);
        else lv_obj_clear_state(objects.spray_state, LV_STATE_CHECKED);
    }

    currentIndex2 = 0;       
    wait_for_release = true; 

    if(objects.set_time != NULL) {
        lv_scr_load_anim(objects.set_time, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    }

    updatePgSwitch2x2(); 
}

void updateButtonSelection() {

    if (lv_scr_act() != objects.pg_work) return;

    lv_obj_t* btn_list[7] = {
        objects.bm_0, objects.bm_1, objects.bm_2, 
        objects.bm_3, objects.bm_4, objects.bm_5, objects.bm_6
    };
        lv_obj_t* btnt_list[7] = {
        objects.bmt_0, objects.bmt_1, objects.bmt_2, 
        objects.bmt_3, objects.bmt_4, objects.bmt_5, objects.bmt_6
    };

    if (btn_list[0] == NULL) return; 
    if (btnt_list[0] == NULL) return; 

    int btnLeft  = digitalRead(bt_3);
    int btnRight = digitalRead(bt_4);
    int btnDown  = digitalRead(bt_7);
    // int btnUp = digitalRead(bt_6); // ï¿½Ñ´ï¿½Í¡ï¿½ï¿½ï¿½ï¿½ï¿½

    unsigned long now = millis();
    int newIndex = curIndex;

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

    if (newIndex < 0) newIndex = 0;
    if (newIndex > 6) newIndex = 6;

    if (newIndex != curIndex) {

        if (curIndex >= 0 && curIndex <= 6) {
            if (btn_list[curIndex] != NULL) {
                lv_obj_clear_state(btn_list[curIndex], LV_STATE_PRESSED);
            }
            if (btnt_list[curIndex] != NULL) {
                lv_obj_clear_state(btnt_list[curIndex], LV_STATE_PRESSED);
            }
        }

        curIndex = newIndex;

        if (curIndex >= 0 && curIndex <= 6) {
            if (btn_list[curIndex] != NULL) {
                lv_obj_add_state(btn_list[curIndex], LV_STATE_PRESSED);
            }
            if (btnt_list[curIndex] != NULL) {
                lv_obj_add_state(btnt_list[curIndex], LV_STATE_PRESSED);
            }
        }
    }
    else {
         if (btn_list[curIndex] != NULL && !lv_obj_has_state(btn_list[curIndex], LV_STATE_PRESSED)) {
             lv_obj_add_state(btn_list[curIndex], LV_STATE_PRESSED);
         }
        if (btnt_list[curIndex] != NULL && !lv_obj_has_state(btnt_list[curIndex], LV_STATE_PRESSED)) {
             lv_obj_add_state(btnt_list[curIndex], LV_STATE_PRESSED);
         }
    }

    if(btn_ent == HIGH){
        switch(curIndex) {
            case 0: Day_pg = "Sunday";    currentDayIndex = 0; break;
            case 1: Day_pg = "Monday";    currentDayIndex = 1; break;
            case 2: Day_pg = "Tuesday";   currentDayIndex = 2; break;
            case 3: Day_pg = "Wednesday"; currentDayIndex = 3; break;
            case 4: Day_pg = "Thursday";  currentDayIndex = 4; break;
            case 5: Day_pg = "Friday";    currentDayIndex = 5; break;
            case 6: Day_pg = "Saturday";  currentDayIndex = 6; break;
        }

        lv_scr_load_anim(objects.pg_switch, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    }
}
const char* getDeviceName(int d) {
  const char* days[] = { "NONE", "FAN", "LIGHT", "PUMP", "SPRAY" };
  return days[d];
}


static lv_obj_t * last_act_scr = NULL;
static unsigned long lastUpdate2 = 0; 
unsigned long globalLastEscTime = 0;
bool wait_page_btn = false;
bool wait_direac_btn = false;
#define CUR_SCREEN lv_scr_act()
void CT_mode() {
    lv_timer_handler(); // ï¿½ï¿½ï¿½ LVGL ï¿½Ó§Ò¹ background
    
    unsigned long now = millis();
    lv_obj_t* activeScreen = lv_scr_act(); // ï¿½Ö§Ë¹ï¿½Ò¨Í»Ñ¨ï¿½ØºÑ¹ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½

    // ---------------------------------------------------------
    // 1. ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Â¹Ë¹ï¿½ï¿½ï¿½ï¿½Ñ¡ (bt_1) - Global Navigation
    // ---------------------------------------------------------
    static bool btn1_prev = false;
    if (digitalRead(bt_1) == HIGH) {
        if (!btn1_prev) {
            btn1_prev = true;
            
            // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ë¹ï¿½ï¿½ï¿½Ë¹ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ë¹ï¿½Ò¶Ñ´ï¿½ï¿½
            if (activeScreen == objects.main && objects.main != NULL) {
                loadScreen(SCREEN_ID_PG_WORK);
                return; // ï¿½Í¡ï¿½Ò¡ï¿½Ñ§ï¿½ï¿½ï¿½Ñ¹ï¿½Ñ¹ï¿½ï¿½
            } 
            else if (activeScreen == objects.pg_work && objects.pg_work != NULL) {
                loadScreen(SCREEN_ID_PG_SENSER);
                return;
            } 
            else if (activeScreen == objects.pg_senser && objects.pg_senser != NULL) {
                loadScreen(SCREEN_ID_MAIN);
                return;
            }
        }
    } else {
        btn1_prev = false;
    }

    // ---------------------------------------------------------
    // 2. Logic ï¿½Â¡ï¿½ï¿½ï¿½Ë¹ï¿½Ò¨ï¿½ (Safe Mode)
    // ---------------------------------------------------------

    // === Ë¹ï¿½ï¿½ MAIN ===
    if (activeScreen == objects.main && objects.main != NULL) {
        state_device();
        checkModeSensor();
        
        // ï¿½ï¿½ï¿½ï¿½ Mode (bt_2)
        static bool btn2_prev = false;
        if (digitalRead(bt_2) == HIGH) {
            if (!btn2_prev) {
                btn2_prev = true;
                if (objects.mode != NULL) { // ï¿½ï¿½ç¤¡ï¿½Í¹ï¿½ï¿½ï¿½
                    if (lv_obj_has_state(objects.mode, LV_STATE_CHECKED)) {
                        lv_obj_clear_state(objects.mode, LV_STATE_CHECKED);
                        if(objects.mode_label) lv_label_set_text(objects.mode_label, "AUTO");
                    } else {
                        lv_obj_add_state(objects.mode, LV_STATE_CHECKED);
                        if(objects.mode_label) lv_label_set_text(objects.mode_label, "DIRECT");
                    }
                }
            }
        } else {
            btn2_prev = false;
        }

        // ï¿½ï¿½Ã¤Çºï¿½ï¿½ï¿½ï¿½Ø»ï¿½Ã³ï¿½ (Manual)
        if (objects.mode != NULL && lv_obj_has_state(objects.mode, LV_STATE_CHECKED)) {
            simple_toggle(objects.fan,objects.panel_fan,  bt_3);
            simple_toggle(objects.light,objects.panel_light, bt_4);
            simple_toggle(objects.pump,objects.panel_pump,  bt_7);
            simple_toggle(objects.spray,objects.panel_spray, bt_6);
            simple_toggle(objects.speed,objects.speed, bt_5);
            simple_toggle(objects.reserve,objects.panel_reserve, bt_20);
            
        }
    }

    // === Ë¹ï¿½ï¿½ WORK ===
    else if (activeScreen == objects.pg_work && objects.pg_work != NULL) {
        updateButtonSelection();
        
        // ï¿½ï¿½ï¿½ï¿½ Enter -> ï¿½ï¿½Ë¹ï¿½ï¿½ Switch
        if (btn_ent == HIGH) {
            currentDayIndex = curIndex; 
            loadScreen(SCREEN_ID_PG_SWITCH);
            return; // ï¿½Ó¤Ñ­!
        }
        
        // ï¿½ï¿½ï¿½ï¿½ ESC -> ï¿½ï¿½Ñº Main
        if (btn_esc == HIGH && now - globalLastEscTime > 500) { 
            globalLastEscTime = now;
            loadScreen(SCREEN_ID_MAIN);
            return;
        }
    }

    // === Ë¹ï¿½ï¿½ SWITCH ===
    else if (activeScreen == objects.pg_switch && objects.pg_switch != NULL) {
        
        // ï¿½ï¿½Å´ï¿½ï¿½ï¿½ï¿½ï¿½Å¤ï¿½ï¿½ï¿½ï¿½ï¿½Ã¡ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ë¹ï¿½Ò¹ï¿½ï¿½ (ï¿½ï¿½ï¿½ flag ï¿½ï¿½ï¿½ï¿½)
        static bool isPgSwitchLoaded = false;
        if (!isPgSwitchLoaded) {
             loadPgSwitchData();
             isPgSwitchLoaded = true;
        }
        // ï¿½ï¿½ï¿½ï¿½ï¿½Ëµï¿½: ï¿½ï¿½Í§ï¿½Ò¨Ñ§ï¿½ï¿½ï¿½ reset 'isPgSwitchLoaded' ï¿½ï¿½ï¿½ï¿½ false ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Í¡ï¿½Ò¡Ë¹ï¿½Ò¹ï¿½ï¿½
        // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ô¸ï¿½ï¿½ï¿½ï¿½Â¡ loadPgSwitchData() ï¿½ï¿½ï¿½ï¿½ï¿½ create_screen_pg_switch() ï¿½Ð§ï¿½ï¿½Â¡ï¿½ï¿½ï¿½
        
        updatePgSwitch(); 
        currentSlotIndex = currentIndex;
        checkButton(currentSlotIndex, currentDayIndex);

        if (needReloadPgSwitch) {
            loadPgSwitchData();
            needReloadPgSwitch = false;
        }
        
        // ï¿½ï¿½ï¿½ï¿½ ESC -> ï¿½ï¿½ÑºË¹ï¿½ï¿½ Work
        if (btn_esc == HIGH && now - globalLastEscTime > 500) { 
            globalLastEscTime = now;
            isPgSwitchLoaded = false; // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ flag (ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½)
            loadScreen(SCREEN_ID_PG_WORK);
            return;
        }
    }

    // === Ë¹ï¿½ï¿½ SET TIME ===
    else if (activeScreen == objects.set_time && objects.set_time != NULL) {
        
        static unsigned long lastUpdateUI = 0;
        if (now - lastUpdateUI > 200) { 
            lastUpdateUI = now;
            // ï¿½ï¿½ï¿½ï¿½ NULL ï¿½ï¿½Í¹ set text ï¿½ï¿½ï¿½ï¿½
            if(objects.pg_switch_day_1) lv_label_set_text(objects.pg_switch_day_1, dayOfWeek(currentDayIndex).c_str());
            if(objects.num_save) lv_label_set_text_fmt(objects.num_save, "%d", edit_slot_id);
        }
        
        updatePgSwitch2x2();

        // ï¿½ï¿½ï¿½ï¿½ ESC -> ï¿½ï¿½ÑºË¹ï¿½ï¿½ Switch
        if (btn_esc == HIGH && now - globalLastEscTime > 500) { 
            globalLastEscTime = now;
            needReloadPgSwitch = true;
            loadScreen(SCREEN_ID_PG_SWITCH);
            return;
        }
    }

    // === Ë¹ï¿½ï¿½ PG TIME ===
    else if (activeScreen == objects.pg_time && objects.pg_time != NULL) {
        updatePgTime(); 
    }

    // === Ë¹ï¿½ï¿½ SENSOR ===
    else if (activeScreen == objects.pg_senser && objects.pg_senser != NULL) {
        
        static unsigned long lastSensorUpdate = 0;
        if (now - lastSensorUpdate > 250) { 
            lastSensorUpdate = now;
            if(objects.current)   lv_label_set_text(objects.current, String(windMS, 2).c_str());
            if(objects.current_1) lv_label_set_text(objects.current_1, String(windMS, 2).c_str());
            if(objects.current_2) lv_label_set_text(objects.current_2, (String(smoothedWindMs, 2) + " M/s").c_str());
        }
        
        updatePgCalibration();
        checkModeSensor();

        // ï¿½ï¿½ï¿½ï¿½ ESC -> ï¿½ï¿½Ñº Main
        if (btn_esc == HIGH && now - globalLastEscTime > 500) {
            globalLastEscTime = now;
            loadScreen(SCREEN_ID_MAIN);
            return;
        }
    }
}
void state_device(){
    sync_state(objects.fan,   LV_STATE_CHECKED);
    sync_state(objects.light, LV_STATE_CHECKED);
    sync_state(objects.pump,  LV_STATE_CHECKED);
    sync_state(objects.spray, LV_STATE_CHECKED);
    sync_state(objects.reserve, LV_STATE_CHECKED);
}
bool isAutoClosing = false;     
bool isAutoClosing_on = false; 
bool autoLedState = LOW;       

void Closing() {
    static unsigned long timerStart = 0;
    static unsigned long timerBlink = 0;

    if (isAutoClosing) {
 
        if (timerStart == 0) {
            timerStart = millis();
            isAutoClosing_on = false; 
        }

        if (millis() - timerStart < 3000) {
            if (millis() - timerBlink >= 200) { 
                timerBlink = millis();
                autoLedState = !autoLedState;
                digitalWrite(led_auto, autoLedState);
            }
        } 
        else {
            digitalWrite(led_auto, LOW); 
            isAutoClosing = false;      
            timerStart = 0;            
        }
    }
}

void Closing_on() {

    static unsigned long timerStart_on = 0;
    static unsigned long timerBlink_on = 0;

    if (isAutoClosing_on) {
        if (timerStart_on == 0) {
            timerStart_on = millis();
            isAutoClosing = false;
        }

        if (millis() - timerStart_on < 3000) {
            if (millis() - timerBlink_on >= 200) {
                timerBlink_on = millis();
                autoLedState = !autoLedState;
                digitalWrite(led_auto, autoLedState);
            }
        } 
        else {
           
            digitalWrite(led_auto, HIGH); 
            isAutoClosing_on = false;    
            timerStart_on = 0; 
        }
    }
    
}

void led_add_state(){
    bool target_fan_state = lv_obj_has_state(objects.fan, LV_STATE_CHECKED);
    static bool stable_fan_state = false;   
    static bool is_fan_blinking = false; 
    static unsigned long fan_blink_start = 0;
    if (target_fan_state != stable_fan_state) {
        if (!is_fan_blinking) {
            is_fan_blinking = true;
            fan_blink_start = millis();
        }
        if (millis() - fan_blink_start < 5000) {
            if ((millis() / 100) % 2 == 0) {
                digitalWrite(led_fan, HIGH);
            } else {
                digitalWrite(led_fan, LOW);
            }
        } 
        else {
            is_fan_blinking = false;
            stable_fan_state = target_fan_state;
        }
    } else {
        is_fan_blinking = false;
        if (stable_fan_state) {
            digitalWrite(led_fan, HIGH);
        } else {
            digitalWrite(led_fan, LOW);
        }
    }
    
    if(lv_obj_has_state(objects.light, LV_STATE_CHECKED)){digitalWrite(led_light,HIGH);}
    else{digitalWrite(led_light,LOW);}
    
    if(lv_obj_has_state(objects.speed, LV_STATE_CHECKED)){Closing_on(); isAutoClosing = true; }
    else{Closing();isAutoClosing_on = true;}

    if(lv_obj_has_state(objects.spray, LV_STATE_CHECKED)){digitalWrite(led_spray,HIGH);}
    else{digitalWrite(led_spray,LOW);}

    if(lv_obj_has_state(objects.pump, LV_STATE_CHECKED)){digitalWrite(led_pump,HIGH);}
    else{digitalWrite(led_pump,LOW);}
}

lv_timer_t * timer_main  = NULL;
lv_timer_t * timer_1     = NULL;
lv_timer_t * timer_2     = NULL;
lv_timer_t * timer_3     = NULL;

// --- Screensaver ---
static lv_obj_t *   ss_overlay  = NULL;
static lv_obj_t *   ss_label    = NULL;
static lv_timer_t * ss_scroll_t = NULL;
bool         ss_active   = false;
unsigned long lastActivityTime = 0;
#define SS_LABEL_SPEED 3                         // pixels per tick
#define SS_TICK_MS     20                        // ms per tick

void screensaver_scroll_cb(lv_timer_t *) {
    if (!ss_label) return;
    lv_coord_t x = lv_obj_get_x(ss_label);
    x += SS_LABEL_SPEED;
    if (x > SCREEN_WIDTH) x = -lv_obj_get_width(ss_label);
    lv_obj_set_x(ss_label, x);
}

void screensaver_enter() {
    if (ss_active) return;
    ss_active = true;
    if (!ss_overlay) {
        ss_overlay = lv_obj_create(lv_layer_top());
        lv_obj_set_size(ss_overlay, SCREEN_WIDTH, SCREEN_HEIGHT);
        lv_obj_set_pos(ss_overlay, 0, 0);
        lv_obj_set_style_bg_color(ss_overlay, lv_color_black(), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(ss_overlay, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(ss_overlay, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_radius(ss_overlay, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_all(ss_overlay, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_clear_flag(ss_overlay, LV_OBJ_FLAG_SCROLLABLE);
        ss_label = lv_label_create(ss_overlay);
        lv_label_set_text(ss_label, "AUTOMATION CLUSTER");
        lv_obj_set_style_text_color(ss_label, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(ss_label, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_align(ss_label, LV_ALIGN_LEFT_MID, -lv_obj_get_width(ss_label), 0);
    }
    lv_obj_clear_flag(ss_overlay, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_x(ss_label, -lv_obj_get_width(ss_label));
    if (!ss_scroll_t) {
        ss_scroll_t = lv_timer_create(screensaver_scroll_cb, SS_TICK_MS, NULL);
    }
}

void screensaver_exit() {
    if (!ss_active) return;
    ss_active = false;
    if (ss_scroll_t) { lv_timer_del(ss_scroll_t); ss_scroll_t = NULL; }
    if (ss_overlay)  { lv_obj_add_flag(ss_overlay, LV_OBJ_FLAG_HIDDEN); }
    lastActivityTime = millis();
}

// --- Alarm overlay (full-screen centered, stacks active alarms vertically) ---
static lv_obj_t * alarm_overlay          = NULL;
static lv_obj_t * alarm_disp_label[3]   = {NULL, NULL, NULL};
static bool       alarm_active[3]        = {false, false, false};
static const char * const alarm_label_text[3] = {"Alram_1", "Alram_2", "Alram_3"};

#define ALARM_ROW_H  60
#define ALARM_PAD    10

void ensure_alarm_overlay() {
    if (alarm_overlay != NULL) return;
    alarm_overlay = lv_obj_create(lv_layer_top());
    lv_obj_set_style_bg_color(alarm_overlay, lv_color_black(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(alarm_overlay, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(alarm_overlay, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(alarm_overlay, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(alarm_overlay, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(alarm_overlay, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(alarm_overlay, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(alarm_overlay, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_size(alarm_overlay, 460, ALARM_ROW_H + ALARM_PAD * 2);
    for (int i = 0; i < 3; i++) {
        alarm_disp_label[i] = lv_label_create(alarm_overlay);
        lv_label_set_text(alarm_disp_label[i], alarm_label_text[i]);
        lv_obj_set_style_text_color(alarm_disp_label[i], lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(alarm_disp_label[i], &lv_font_montserrat_40, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_size(alarm_disp_label[i], 460, ALARM_ROW_H);
        lv_obj_set_style_text_align(alarm_disp_label[i], LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_opa(alarm_disp_label[i], LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_add_flag(alarm_disp_label[i], LV_OBJ_FLAG_HIDDEN);
    }
}

void update_alarm_overlay() {
    if (alarm_overlay == NULL) return;
    int count = 0;
    for (int i = 0; i < 3; i++) if (alarm_active[i]) count++;
    if (count == 0) {
        lv_obj_add_flag(alarm_overlay, LV_OBJ_FLAG_HIDDEN);
        return;
    }
    lv_obj_clear_flag(alarm_overlay, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_height(alarm_overlay, count * ALARM_ROW_H + ALARM_PAD * (count + 1));
    lv_obj_align(alarm_overlay, LV_ALIGN_CENTER, 0, 0);
    int row = 0;
    for (int i = 0; i < 3; i++) {
        if (alarm_active[i]) {
            lv_obj_clear_flag(alarm_disp_label[i], LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_pos(alarm_disp_label[i], 0, ALARM_PAD + row * (ALARM_ROW_H + ALARM_PAD));
            row++;
        } else {
            lv_obj_add_flag(alarm_disp_label[i], LV_OBJ_FLAG_HIDDEN);
        }
    }
}

void blink_specific_cb(lv_timer_t * t) {
    intptr_t idx = (intptr_t)lv_timer_get_user_data(t);
    if (idx < 0 || idx > 2) return;
    if (alarm_disp_label[idx] == NULL) return;
    lv_opa_t cur = lv_obj_get_style_opa(alarm_disp_label[idx], LV_PART_MAIN);
    lv_obj_set_style_opa(alarm_disp_label[idx],
        (cur >= LV_OPA_50) ? LV_OPA_TRANSP : LV_OPA_COVER,
        LV_PART_MAIN | LV_STATE_DEFAULT);
}

void stop_blink(lv_timer_t * &tmr, lv_obj_t * obj) {
    if(tmr != NULL) {
        lv_timer_del(tmr);
        tmr = NULL;
    }
    if(obj != NULL) lv_obj_clear_state(obj, LV_STATE_CHECKED);
}

int leds[] = {led_fan, led_light, led_auto, led_spray, led_pump};
int numLeds = 5;
void Read_nano(){
            if (Serial2.available()) {
            String incomingData = Serial2.readStringUntil('\n');
            incomingData.trim();
            // --- Alarm 1 ---
            if(incomingData == "Alarm_1_ON") {
                ensure_alarm_overlay();
                alarm_active[0] = true;
                update_alarm_overlay();
                if(timer_1 == NULL) timer_1 = lv_timer_create(blink_specific_cb, 500, (void*)(intptr_t)0);
            }
            if(incomingData == "Alarm_1_OFF") {
                stop_blink(timer_1, objects.alram_1);
                alarm_active[0] = false;
                if (alarm_disp_label[0]) lv_obj_set_style_opa(alarm_disp_label[0], LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
                update_alarm_overlay();
            }
            // --- Alarm 2 ---
            if(incomingData == "Alarm_2_ON") {
                ensure_alarm_overlay();
                alarm_active[1] = true;
                update_alarm_overlay();
                if(timer_2 == NULL) timer_2 = lv_timer_create(blink_specific_cb, 500, (void*)(intptr_t)1);
            }
            if(incomingData == "Alarm_2_OFF") {
                stop_blink(timer_2, objects.alram_2);
                alarm_active[1] = false;
                if (alarm_disp_label[1]) lv_obj_set_style_opa(alarm_disp_label[1], LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
                update_alarm_overlay();
            }
            // --- Alarm 3 ---
            if(incomingData == "Alarm_3_ON") {
                ensure_alarm_overlay();
                alarm_active[2] = true;
                update_alarm_overlay();
                if(timer_3 == NULL) timer_3 = lv_timer_create(blink_specific_cb, 500, (void*)(intptr_t)2);
            }
            if(incomingData == "Alarm_3_OFF") {
                stop_blink(timer_3, objects.alram_3);
                alarm_active[2] = false;
                if (alarm_disp_label[2]) lv_obj_set_style_opa(alarm_disp_label[2], LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
                update_alarm_overlay();
            }
        }
}

bool isSystemOn = false; 
unsigned long pressStartTime_btn = 0;
bool isBtnPressed = false;
bool actionTriggered = false;
unsigned long taskTimer = 0;
int counter = 0;
static unsigned long activationStartTime = 0;
int led_time = 21;
#include "esp_task_wdt.h"
void setup()
{

    tft.init();
    tft.begin();
    tft.setRotation(3); 
    tft.setBrightness(0);
    tft.fillScreen(TFT_BLACK);
    loadAlertSetting();
    loadCalibration();
    Wire.begin(8, 9);
    for (int i = 0; i < numLeds; i++) {
    pinMode(leds[i], OUTPUT);
  }
    pinMode(led_time,OUTPUT);
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
     pinMode(bt_20, INPUT);
    pinMode(OutPin, INPUT);
    
    gpio_reset_pin(GPIO_NUM_1);
    pinMode(led1,INPUT);
    Serial.begin(115200);
    Serial2.begin(19200, SERIAL_8N1, RX_PIN, TX_PIN);
    Serial2.println("restart"); 
    Serial.setTimeout(10);
    Serial.printf("RESET REASON: %d\n", esp_reset_reason());
    if (!rtc.begin()) {
        Serial.println("RTC NOT FOUND!");
    }
    initMemory();
    lv_init();
    lv_tick_set_cb(millis);
    static lv_color_t buf[ SCREEN_WIDTH * 10 ];
    lv_display_t *disp = lv_display_create(SCREEN_WIDTH, SCREEN_HEIGHT);
    lv_display_set_flush_cb(disp, my_disp_flush);
    lv_display_set_buffers(disp, buf, NULL, sizeof(buf), LV_DISPLAY_RENDER_MODE_PARTIAL);

    ui_init();
    Serial.println("Setup Done");
    sr.setAllHigh();
        digitalWrite(led_time, LOW);
        digitalWrite(led_fan, LOW);
        digitalWrite(led_light, LOW);
        digitalWrite(led_auto, LOW);
        digitalWrite(led_pump, LOW);
        digitalWrite(led_spray, LOW);
    lv_obj_clear_state(objects.fan, LV_STATE_CHECKED);
    lv_obj_clear_state(objects.light, LV_STATE_CHECKED);
    lv_obj_clear_state(objects.pump, LV_STATE_CHECKED);
    lv_obj_clear_state(objects.spray, LV_STATE_CHECKED);
    lv_obj_clear_state(objects.reserve, LV_STATE_CHECKED);

    lv_obj_clear_state(objects.panel_fan, LV_STATE_CHECKED);
    lv_obj_clear_state(objects.panel_light, LV_STATE_CHECKED);
    lv_obj_clear_state(objects.panel_pump, LV_STATE_CHECKED);
    lv_obj_clear_state(objects.panel_spray, LV_STATE_CHECKED);
    lv_obj_clear_state(objects.panel_reserve, LV_STATE_CHECKED);
    lv_obj_clear_state(objects.speed, LV_STATE_CHECKED);
    state_device();
    Serial2.println("restart");
// delay(2000); // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ Serial Monitor ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ó§Ò¹

//     // 1. ï¿½ï¿½ï¿½Ò§ï¿½ï¿½ï¿½ï¿½ï¿½Å¨ï¿½ï¿½Í§ (ï¿½ï¿½ï¿½ index 0 ï¿½Ö§ 5 ï¿½ï¿½ï¿½Ò¹ï¿½ï¿½ï¿½!)
//     ProgramData p;
//     for(int i = 0; i < 6; i++) {
//         p.fan[i] = 0; p.light[i] = 0; p.spray[i] = 0; p.pump[i] = 0;
//         p.hour_start[i] = 0; p.minute_start[i] = 0;
//         p.hour_end[i] = 0;   p.minute_end[i] = 0;
//         p.state[i] = 0;
//     }

//     // 2. ï¿½Ù»ï¿½Ñ¹ï¿½Ö¡ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ò§ Namespace ï¿½ï¿½ NVS (ï¿½ï¿½ï¿½ï¿½Ñ­ï¿½ï¿½ NOT_FOUND)
//     Serial.println("Starting NVS Initialization...");
//     for (int d = 0; d <= 6; d++) {       // ï¿½Ñ¹ï¿½ï¿½ï¿½ 0-6 (7 ï¿½Ñ¹)
//         for (int s = 0; s < 6; s++) {    // Slot 0-5 (ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ 6 ï¿½ï¿½ï¿½ï¿½Ð¨ï¿½ï¿½ï¿½Ô´ Stack Smashing)
//             saveSingleSlot(d, s, p);
//             delay(10); // ï¿½ï¿½Í§ï¿½Ñ¹ Watchdog Reset
//         }
//         Serial.printf("Day %d initialized...\n", d);
//     }
//     Serial.println("All NVS Data Initialized Successfully!");

//     // 3. ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ I2C ï¿½ï¿½Í¨Ò¡ï¿½ï¿½ï¿½
//     if (!rtc.begin()) {
//         Serial.println("Couldn't find RTC");
//     }
// saveTimeSettings(16,1,2026,14,24);
        //   saveLow(9699.95, 0.00);
        //   saveHigh(13173.06, 2.00);
        //   saveAlertSetting(1.0);


}

unsigned long last = 0;
const unsigned long print = 1000;
// --- ï¿½Ñ§ï¿½ï¿½ï¿½Ñ¹ï¿½ï¿½ï¿½ï¿½Ñº Monitor ï¿½Ðºï¿½ (ï¿½ï¿½ï¿½á·¹ vTaskList ï¿½ï¿½ï¿½ Error) ---
void printSystemStatus() {
    static uint32_t lastPrint = 0;
    if (millis() - lastPrint < 2000) return; // ï¿½Ê´ï¿½ï¿½Å·Ø¡ 2 ï¿½Ô¹Ò·ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ã¡ Serial
    lastPrint = millis();

    Serial.println(F("\n--- [ SYSTEM MONITOR ] ---"));
    
    // 1. ï¿½ï¿½Ç¨ï¿½Íº RAM (Heap)
    Serial.printf("Free Heap: %u bytes\n", ESP.getFreeHeap());
    Serial.printf("Min Free Heap: %u bytes (ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Íµï¿½ï¿½ï¿½Ø´)\n", ESP.getMinFreeHeap());

    // 2. ï¿½ï¿½Ç¨ï¿½Íº Stack ï¿½Í§ loopTask (ï¿½Ø´ï¿½ï¿½ï¿½ï¿½Ñ¡ï¿½Ð·ï¿½ï¿½ï¿½ï¿½ WDT Trigger)
    // ï¿½ï¿½Òµï¿½ï¿½ï¿½Å¢ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ 0 ï¿½Ê´ï¿½ï¿½ï¿½ï¿½ Stack Overflow
    Serial.printf("LoopTask Stack Left: %u bytes\n", uxTaskGetStackHighWaterMark(NULL));

    // 3. ï¿½ï¿½Ç¨ï¿½Íº Uptime
    Serial.printf("Uptime: %lu seconds\n", millis() / 1000);
    Serial.println(F("--------------------------"));
}
unsigned long mainScreenEnterTime = 0;
bool isOnMainScreen = false;
bool hasReadWind = false;
// ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ê¶Ò¹Ð»ï¿½ï¿½ï¿½
unsigned long pressStartTime_time = 0;
bool isTime = false;
bool isbtnTime = false;
bool actionTriggered_time = false;


void loop(){
    static uint32_t lastNano = 0;
    static uint32_t lastRTC  = 0;
    static uint32_t lastWind = 0;
    static uint32_t lastLVGL = 0;
    if (lastActivityTime == 0) lastActivityTime = millis();
    bt();
    // Check screensaver timeout (skip if alarm is active)
    if (!ss_active && (millis() - lastActivityTime >= SS_TIMEOUT_MS)) {
        screensaver_enter();
    }
    checkLongPress();
    checkResetButton();
     if (!isSystemOn){checkLongPress_time();}
    if (!isSystemOn && !isTime) { 
        vTaskDelay(10); 
        return;
    }
    led_add_state();
    handle_mute_button();
    handleShortPressTask();
    digitalWrite(led_time, (lv_obj_has_state(objects.mode, LV_STATE_CHECKED)) ? HIGH : LOW);
    if (activationStartTime == 0) activationStartTime = millis();
    if (millis() - activationStartTime < numplecs) {
        if (CUR_SCREEN == objects.main) {
            lv_obj_set_style_text_font(objects.senser, &ui_font_thai_18, 0);
            lv_label_set_text(objects.senser, "Wait..");
            hasReadWind = false; 
        }
    }
        // ï¿½ï¿½Ç¹ï¿½Í§ï¿½ï¿½Ã¨Ñ´ï¿½ï¿½ï¿½Ë¹ï¿½Ò¨ï¿½ï¿½ï¿½Ñ¡ï¿½ï¿½Ð¿Ñ§ï¿½ï¿½ï¿½Ñ¹ï¿½ï¿½ï¿½ï¿½ï¿½Í§ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Í¹ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
    if (millis() - lastRTC >= 1000) {
        lastRTC = millis();
        timer();
        
        if (CUR_SCREEN == objects.main) {
            if (!isOnMainScreen) {
                isOnMainScreen = true;
                mainScreenEnterTime = millis();
            }
            
            // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Í¹ï¿½ï¿½ && hasReadWind ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ readWind() ï¿½Ó§Ò¹ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ò§ï¿½ï¿½ï¿½ï¿½ 1 ï¿½ï¿½ï¿½ï¿½ï¿½
            if (millis() - mainScreenEnterTime > 3000 && hasReadWind) {
                recheckupdata();
                checkAndRunSchedule();
                
                // ï¿½Ò¡ï¿½ï¿½Í§ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Í¡ï¿½ï¿½ï¿½ï¿½Ò¹ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Íºï¿½Ñ´ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½Ã¶ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ flag ï¿½Ã§ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
                // hasReadWind = false; 
            }
        } else {
            isOnMainScreen = false;
            // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Í¡ï¿½Ò¡Ë¹ï¿½ï¿½ï¿½ï¿½Ñ¡ ï¿½Ò¨ï¿½Ðµï¿½Í§ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ flag ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ñºï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Í§ï¿½ï¿½Ã¢Í§ï¿½Ðºï¿½
            
        }
    }

    CT_mode();

    if (millis() - lastNano >= 50) {
        lastNano = millis();
        Read_nano();
    }

    if (millis() - lastWind >= 30) {
        lastWind = millis();
        if (millis() - activationStartTime >= numplecs && CUR_SCREEN != objects.pg_time) {
            readWind();
            hasReadWind = true; 
        }
    }
    if (millis() - lastLVGL >= 5) {
        lastLVGL = millis();
        lv_timer_handler();
    }
    vTaskDelay(1);
}

void checkLongPress_time() {
    if (digitalRead(bt_2) == HIGH) {
        if (!isbtnTime) {
            isbtnTime = true;
            pressStartTime_time = millis();
        }
        if (!actionTriggered_time && (millis() - pressStartTime_time >= 3000)) { // Å´ï¿½ï¿½ï¿½ï¿½ï¿½ 3 ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½Í¤ï¿½ï¿½ï¿½ï¿½ï¿½ (User Experience)
            isTime = !isTime; 
            if (isTime) {
                tft.setBrightness(255);
                lv_scr_load_anim(objects.pg_time, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
            } else {
                isSystemOn = false;
                tft.fillScreen(TFT_BLACK);
                shutDownScreen();
            }
            actionTriggered_time = true;
        }
    } else {
        isbtnTime = false;
        actionTriggered_time = false;
    }
}

void checkLongPress() {
    if (btn_ent == HIGH) {
        if (!isBtnPressed) {
            isBtnPressed = true;
            pressStartTime_btn = millis();
        }

        if (!actionTriggered && (millis() - pressStartTime_btn >= 3000)) { // Å´ï¿½ï¿½ï¿½ï¿½ï¿½ 3 ï¿½ï¿½
            isSystemOn = !isSystemOn;
            actionTriggered = true;

            if (isSystemOn) {
                wakeUpScreen();
            } else {
               shutDownScreen();
            }
        }
    } else {
        isBtnPressed = false;
        actionTriggered = false;
    }
}

// --- ï¿½Ñ§ï¿½ï¿½ï¿½Ñ¹ Wake Up ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ (Å´ï¿½ï¿½ï¿½ï¿½ Blocking) ---
void wakeUpScreen() {
     isOnMainScreen = false;
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.setTextDatum(middle_center);

    int cx = tft.width() / 2;
    int cy = tft.height() / 2;

    tft.drawString("SYSTEM: ON", cx, cy - 40);
    tft.drawString("Ready...", cx, cy - 10);

    int barWidth = 200;
    int barHeight = 24;
    int barX = cx - (barWidth / 2);
    int barY = cy + 20;

    tft.drawRect(barX, barY, barWidth, barHeight, TFT_WHITE);
  String loadingText = "COMPLETED";
   for (int i = 0; i <= 100; i++) {
    int currentW = map(i, 0, 100, 0, barWidth - 4);
    tft.fillRect(barX + 2, barY + 2, currentW, barHeight - 4, TFT_GREEN);
    tft.setClipRect(barX + 2, barY + 2, currentW, barHeight - 4);
    tft.setTextColor(TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextDatum(middle_center);
    tft.drawString(loadingText, barX + (barWidth / 2), barY + (barHeight / 2));
    tft.clearClipRect();
    delay(100);
    lv_obj_set_style_text_font(objects.m_fan, &ui_font_thai_18, 0);
    lv_label_set_text(objects.m_fan, "Wait");
    is_wind_zero_state = false; 
  }
    tft.setTextDatum(top_left);
    lv_scr_load_anim(objects.main, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    lv_obj_clear_state(objects.sound_1, LV_STATE_CHECKED);
    counter = 0;
}

void shutDownScreen() {

    tft.fillScreen(TFT_BLACK);
    tft.setBrightness(0);
    sr.setAllHigh(); // Relay logic (Active LOW/HIGH?) ï¿½ï¿½Ç¨ï¿½Íºï¿½ï¿½ï¿½ High ï¿½ï¿½Í»Ô´ï¿½ï¿½Ô§ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½

    gpio_reset_pin(GPIO_NUM_1);
    pinMode(led1,INPUT);
    // ï¿½Ô´ LED ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
    digitalWrite(led_time, LOW);
    digitalWrite(led_fan, LOW);
    digitalWrite(led_light, LOW);
    digitalWrite(led_auto, LOW);
    digitalWrite(led_pump, LOW);
    digitalWrite(led_spray, LOW);
    activationStartTime = 0;
    // Clear LVGL States
    lv_obj_clear_state(objects.fan, LV_STATE_CHECKED);
    lv_obj_clear_state(objects.light, LV_STATE_CHECKED);
    lv_obj_clear_state(objects.pump, LV_STATE_CHECKED);
    lv_obj_clear_state(objects.spray, LV_STATE_CHECKED);
    lv_obj_clear_state(objects.speed, LV_STATE_CHECKED);
    lv_obj_clear_state(objects.reserve, LV_STATE_CHECKED);

    lv_obj_clear_state(objects.panel_fan, LV_STATE_CHECKED);
    lv_obj_clear_state(objects.panel_light, LV_STATE_CHECKED);
    lv_obj_clear_state(objects.panel_pump, LV_STATE_CHECKED);
    lv_obj_clear_state(objects.panel_spray, LV_STATE_CHECKED);
    lv_obj_clear_state(objects.panel_reserve, LV_STATE_CHECKED);
    state_device();
    Serial2.println("restart"); 
    hasReadWind = false;
    delay(500);
}
void checkResetButton() {
  static unsigned long esc_start_time = 0;
  static bool is_esc_pressing = false;

  // ï¿½ï¿½Ç¨ï¿½Íºï¿½ï¿½Ò»ï¿½ï¿½ï¿½ï¿½Ù¡ï¿½ï¿½ (ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ btn_esc ï¿½ï¿½ï¿½Ê¶Ò¹Ð·ï¿½ï¿½ï¿½ï¿½Ò¹ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ digitalRead)
  if (btn_esc == HIGH) { 
    
    if (!is_esc_pressing) {
      is_esc_pressing = true;
      esc_start_time = millis(); 
    }

    // ï¿½ï¿½ï¿½ä¢¨Ò¡ 3000 ï¿½ï¿½ï¿½ï¿½ 5000 (Ë¹ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ô¹Ò·ï¿½)
    if (millis() - esc_start_time >= 5000) {
      Serial2.println("System Restarting..."); // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Í¹ï¿½ï¿½Í¹ï¿½ï¿½Êµï¿½ï¿½ï¿½ï¿½
      delay(100); // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ Serial ï¿½è§¢ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Í¡ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ãºï¿½ï¿½Í¹
      ESP.restart();
      
    }
    
  } else {
    // ï¿½ï¿½ï¿½ï¿½Í»ï¿½ï¿½ï¿½Â»ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ê¶Ò¹ï¿½
    is_esc_pressing = false;
  }
}


void printHardwareSpecs(HardwareSerial &serialPort) {
  serialPort.println("\n======================================");
  serialPort.println("      ESP32-S3 HARDWARE REPORT        ");
  serialPort.println("======================================");
  
  serialPort.printf("Chip Model:    %s\n", ESP.getChipModel());
  serialPort.printf("Chip Revision: %d\n", ESP.getChipRevision());
  serialPort.printf("CPU Cores:     %d\n", ESP.getChipCores());
  serialPort.printf("CPU Speed:     %d MHz\n", ESP.getCpuFreqMHz());
  
  serialPort.println("--------------------------------------");
  serialPort.printf("Flash Size:    %d MB\n", ESP.getFlashChipSize() / (1024 * 1024));
  serialPort.printf("Flash Speed:   %d MHz\n", ESP.getFlashChipSpeed() / 1000000);
  
  serialPort.println("--------------------------------------");
  // ï¿½ï¿½Ç¨ï¿½Íº RAM ï¿½ï¿½ï¿½ï¿½ï¿½ (Internal SRAM)
  serialPort.printf("Internal RAM:  %.2f KB\n", ESP.getHeapSize() / 1024.0);
  serialPort.printf("Free Heap:     %.2f KB\n", ESP.getFreeHeap() / 1024.0);

  // ï¿½ï¿½Ç¨ï¿½Íº PSRAM (External RAM)
  if (psramFound()) {
    serialPort.printf("PSRAM Size:    %ld MB\n", ESP.getPsramSize() / (1024 * 1024));
    serialPort.printf("Free PSRAM:    %ld KB\n", ESP.getFreePsram() / 1024);
  } else {
    serialPort.println("PSRAM:         Not Found / Not Enabled");
  }
  serialPort.println("======================================");
}
void monitorSystem() {
    static uint32_t lastMon = 0;
    if (millis() - lastMon < 2000) return; // ï¿½ï¿½ç¤·Ø¡ 2 ï¿½Ô¹Ò·ï¿½
    lastMon = millis();

    Serial.println("--- System Monitor ---");
    // 1. ï¿½ï¿½ RAM ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ (Heap)
    Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
    
    // 2. ï¿½Ù¤ï¿½ï¿½ Stack ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Í¢Í§ Task ï¿½Ñ¨ï¿½ØºÑ¹ (loopTask)
    // ï¿½ï¿½Òµï¿½ï¿½ï¿½Å¢ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ 0 ï¿½Ê´ï¿½ï¿½ï¿½ï¿½ Stack ï¿½ï¿½ï¿½Ñ§ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ (Stack Overflow)
    Serial.printf("LoopTask Stack Left: %d bytes\n", uxTaskGetStackHighWaterMark(NULL));
    
    // 3. ï¿½ï¿½ï¿½ï¿½ÒµÍ¹ï¿½ï¿½ï¿½ï¿½Ñ¹ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ CPU Core ï¿½Ë¹ (ESP32-S3 ï¿½ï¿½ 0 ï¿½ï¿½ï¿½ 1)
    Serial.printf("Running on Core: %d\n", xPortGetCoreID());
    Serial.println("----------------------");
}
