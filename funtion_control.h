#ifndef FUNTION_CONTROL_H
#define FUNTION_CONTROL_H

#include <RTClib.h>
#include <lvgl.h>
#include "LGFX_ILI9488_S3.hpp"
LGFX tft;
// 1. อ้างอิงตัวแปรจากไฟล์หลัก และไฟล์ UI
extern RTC_DS3231 rtc;          
extern objects_t objects; 

// 2. โครงสร้างสถานะสำหรับคำนวณ
struct CalculatedState {
    bool fan = false;
    bool light = false;
    bool pump = false;
    bool spray = false;
};

// 3. ตัวแปร Global สำหรับพักข้อมูล (Cache)
inline ProgramData cachedTodayData;
inline int cachedLoadedDay = -1;
inline uint32_t lastCacheUpdateMillis = 0; 

// --- ฟังก์ชันโหลดข้อมูลโปรแกรมมาเก็บไว้ใน RAM ---
void updateGlobalScheduleCache() {
    DateTime now = rtc.now();
    cachedLoadedDay = now.dayOfTheWeek();
    cachedTodayData = loadProgram(cachedLoadedDay); 

    lastCacheUpdateMillis = millis(); 

    Serial.print("Update Cache: Day ");
    Serial.print(cachedLoadedDay);
    Serial.println(" (Updated every 10 mins)");
}

// --- ฟังก์ชันคำนวณช่วงเวลา ---
inline CalculatedState getScheduleStateFromCache(int currentMinutes) {
    CalculatedState result; 
    for (int i = 0; i < 6; i++) {
        if (cachedTodayData.state[i] == 1) {
            int startMinutes = (cachedTodayData.hour_start[i] * 60) + cachedTodayData.minute_start[i];
            int endMinutes = (cachedTodayData.hour_end[i] * 60) + cachedTodayData.minute_end[i];
            
            bool isTime = false;
            if (startMinutes < endMinutes) {
                if (currentMinutes >= startMinutes && currentMinutes < endMinutes) isTime = true;
            } else { 
                if (currentMinutes >= startMinutes || currentMinutes < endMinutes) isTime = true;
            }

            if (isTime) {
                if(cachedTodayData.fan[i] == 1)   result.fan = true;
                if(cachedTodayData.light[i] == 1) result.light = true;
                if(cachedTodayData.pump[i] == 1)  result.pump = true;
                if(cachedTodayData.spray[i] == 1) result.spray = true;
            }
        }
    }
    return result;
}

void controlDevice(int deviceID, bool turnOn, lv_obj_t* ui_obj) {
    if (ui_obj == NULL) return;

    if(turnOn) {
        if(!lv_obj_has_state(ui_obj, LV_STATE_CHECKED)) lv_obj_add_state(ui_obj, LV_STATE_CHECKED);
    } else {
        if(lv_obj_has_state(ui_obj, LV_STATE_CHECKED)) lv_obj_clear_state(ui_obj, LV_STATE_CHECKED);
    }

    switch (deviceID) {
        case 1: Serial2.println(turnOn ? "FAN_ON" : "FAN_OFF"); break;
        case 2: Serial2.println(turnOn ? "LIGHT_ON" : "LIGHT_OFF"); break;
        case 3: Serial2.println(turnOn ? "PUMP_ON" : "PUMP_OFF"); break;
        case 4: Serial2.println(turnOn ? "SPRAY_ON" : "SPRAY_OFF"); break;
    }
}

void checkAndRunSchedule() {

    if (objects.mode == NULL) return; 

    DateTime now = rtc.now();
    int currentDay = now.dayOfTheWeek();
    int currentMinutes = (now.hour() * 60) + now.minute();
    uint32_t currentMillis = millis();

    if (currentDay != cachedLoadedDay || (currentMillis - lastCacheUpdateMillis >= 60000)) {
        updateGlobalScheduleCache();
    }

    if (lv_obj_has_state(objects.mode, LV_STATE_CHECKED)) {
        return; 
    }

    CalculatedState target = getScheduleStateFromCache(currentMinutes);

    controlDevice(1, target.fan,   objects.fan);
    controlDevice(2, target.light, objects.light);
    controlDevice(3, target.pump,  objects.pump);
    controlDevice(4, target.spray, objects.spray);
}

// 1. ประกาศตัวแปร Global ไว้ด้านบนสุด (นอก void loop)
unsigned long timerPressStart = 0;  // เก็บเวลาที่เริ่มกด
bool isTriggered = false;           // กันไม่ให้คำสั่งทำงานรัวๆ เมื่อกดค้างยาวเกิน 3 วิ

void showProcessScreen(String title, String status, String barText, int duration_ms, uint16_t barColor) {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.setTextDatum(middle_center);

    int cx = tft.width() / 2;
    int cy = tft.height() / 2;

    // แสดงหัวข้อ
    tft.drawString(title, cx, cy - 40);
    tft.drawString(status, cx, cy - 10);

    int barWidth = 200;
    int barHeight = 24;
    int barX = cx - (barWidth / 2);
    int barY = cy + 20;

    // วาดกรอบบาร์
    tft.drawRect(barX, barY, barWidth, barHeight, TFT_WHITE);

    // คำนวณ delay ต่อ 1% (เพื่อให้เวลาโดยรวมเท่ากับ duration_ms)
    int stepDelay = duration_ms / 100; 

    for (int i = 0; i <= 100; i++) {
        int currentW = map(i, 0, 100, 0, barWidth - 4);
        
        // วาดสีพื้นหลังในบาร์
        tft.fillRect(barX + 2, barY + 2, currentW, barHeight - 4, barColor);
        
        // เทคนิค ClipRect: วาดข้อความเฉพาะส่วนที่บาร์สีทับอยู่
        tft.setClipRect(barX + 2, barY + 2, currentW, barHeight - 4);
        
        tft.setTextColor(TFT_BLACK); // สีข้อความในบาร์
        tft.setTextSize(2);
        tft.setTextDatum(middle_center);
        tft.drawString(barText, barX + (barWidth / 2), barY + (barHeight / 2));
        
        tft.clearClipRect();
        
        delay(stepDelay); // ใช้ delay ที่คำนวณมา
    }
    
    // คืนค่าการจัดวางข้อความ
    tft.setTextDatum(top_left);
}



#endif