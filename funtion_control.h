#ifndef FUNTION_CONTROL_H
#define FUNTION_CONTROL_H

#include <RTClib.h>
#include <lvgl.h>
#include "LGFX_ILI9488_S3.hpp"
// [BUG 6] LGFX tft is now defined in fumhood.ino; only declare it here
extern LGFX tft;
// 1. อ้างอิงตัวแปรจากไฟล์หลัก และไฟล์ UI
extern RTC_DS3231 rtc;          
extern objects_t objects; 
#include <string.h>
// ---------------------------------------------------------
// ส่วนประกาศ Struct และตัวแปร Global
// ---------------------------------------------------------

struct CalculatedState {
    bool fan = false;
    bool light = false;
    bool pump = false;
    bool spray = false;
    bool reserve = false; // <--- [เพิ่ม] reserve
};

// ตัวแปร Global สำหรับพักข้อมูล (Cache)
inline ProgramData cachedTodayData;
inline int cachedLoadedDay = -1;
inline uint32_t lastCacheUpdateMillis = 0; 

// ---------------------------------------------------------
// ฟังก์ชันจัดการ Cache และโหลดข้อมูล
// ---------------------------------------------------------

// ฟังก์ชันโหลดข้อมูลโปรแกรมมาเก็บไว้ใน RAM
void updateGlobalScheduleCache() {
    DateTime now = rtc.now();
    cachedLoadedDay = now.dayOfTheWeek();
    
    // [แก้ไข 1] ล้างข้อมูลเก่าในตัวแปร cachedTodayData ให้เป็น 0 ทั้งหมดก่อน
    // เพื่อป้องกันค่าค้าง หรือค่าขยะจากการโหลดครั้งก่อนหน้า
    memset(&cachedTodayData, 0, sizeof(ProgramData));

    // โหลดข้อมูลใหม่จากฟังก์ชัน loadProgram (เช่น จาก EEPROM หรือ SD Card)
    cachedTodayData = loadProgram(cachedLoadedDay); 
    // Serial.printf("=== Loaded Program Data for Day %d ===\n", cachedLoadedDay);
    // for(int i = 0; i < 6; i++) {
    //     Serial.printf("Slot %d [State:%d] | Start: %02d:%02d | End: %02d:%02d | Fan:%d Light:%d Pump:%d Spray:%d Res:%d\n",
    //         i,
    //         cachedTodayData.state[i],
    //         cachedTodayData.hour_start[i], cachedTodayData.minute_start[i],
    //         cachedTodayData.hour_end[i],   cachedTodayData.minute_end[i],
    //         cachedTodayData.fan[i],        cachedTodayData.light[i],
    //         cachedTodayData.pump[i],       cachedTodayData.spray[i],
    //         cachedTodayData.reserve[i]
    //     );
    // }
    // Serial.println("==========================================");
    lastCacheUpdateMillis = millis(); 

    // Serial.print("Update Cache: Day ");
    // Serial.print(cachedLoadedDay);
    // Serial.println(" (Cleared old data & Loaded new)");
}

// ฟังก์ชันสำหรับสั่งให้ระบบรู้ว่าต้องโหลดข้อมูลใหม่ (Force Update)
void recheckupdata(){
    cachedLoadedDay = -1;

    lastCacheUpdateMillis = 0; 
    
    // Serial.println("Force Re-check requested.");
}

// ---------------------------------------------------------
// ฟังก์ชันคำนวณและควบคุมการทำงาน
// ---------------------------------------------------------

inline CalculatedState getScheduleStateFromCache(int currentMinutes) {
    CalculatedState result; 
    // กำหนดค่าเริ่มต้นเป็น false ทั้งหมด เพื่อความชัวร์
    result.fan = false;
    result.light = false;
    result.pump = false;
    result.spray = false;
    result.reserve = false; // <--- [เพิ่ม] ตั้งค่าเริ่มต้น reserve

    for (int i = 0; i < 6; i++) {
        if (cachedTodayData.state[i] == 1) {
            int startMinutes = (cachedTodayData.hour_start[i] * 60) + cachedTodayData.minute_start[i];
            int endMinutes = (cachedTodayData.hour_end[i] * 60) + cachedTodayData.minute_end[i];
            
            bool isTime = false;
            if (startMinutes < endMinutes) {
                if (currentMinutes >= startMinutes && currentMinutes < endMinutes) isTime = true;
            } else { 
                // กรณีข้ามวัน (เช่น 23:00 ถึง 01:00)
                if (currentMinutes >= startMinutes || currentMinutes < endMinutes) isTime = true;
            }

            if (isTime) {
                if(cachedTodayData.fan[i] == 1)   result.fan = true;
                if(cachedTodayData.light[i] == 1) result.light = true;
                if(cachedTodayData.pump[i] == 1)  result.pump = true;
                if(cachedTodayData.spray[i] == 1) result.spray = true;
                if(cachedTodayData.reserve[i] == 1) result.reserve = true; // <--- [เพิ่ม] ประมวลผล reserve
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

    // ส่วนส่ง Serial2
    switch (deviceID) {
        case 1: Serial2.println(turnOn ? "FAN_ON" : "FAN_OFF"); break;
        case 2: Serial2.println(turnOn ? "LIGHT_ON" : "LIGHT_OFF"); break;
        case 3: Serial2.println(turnOn ? "PUMP_ON" : "PUMP_OFF"); break;
        case 4: Serial2.println(turnOn ? "SPRAY_ON" : "SPRAY_OFF"); break;
        case 5: Serial2.println(turnOn ? "RESERVE_ON" : "RESERVE_OFF"); break; // <--- [เพิ่ม] ส่งคำสั่ง reserve ออก Serial2
    }
}

void checkAndRunSchedule() {
    if (objects.mode == NULL) return; 

    DateTime now = rtc.now();
    // int currentDay = now.dayOfTheWeek(); // ไม่ได้ใช้ เพราะใช้ cachedLoadedDay แล้ว
    int currentMinutes = (now.hour() * 60) + now.minute();
    uint32_t currentMillis = millis();

    // เงื่อนไขนี้จะทำงานเมื่อครบ 30 วิ หรือเมื่อถูกสั่งจาก recheckupdata()
    if(currentMillis - lastCacheUpdateMillis >= 30000 || lastCacheUpdateMillis == 0){
        updateGlobalScheduleCache();
    }

    // ถ้าอยู่ในโหมด Manual (Switch ON) ให้ข้ามการทำงาน Auto
    if (lv_obj_has_state(objects.mode, LV_STATE_CHECKED)) {
        return; 
    }

    CalculatedState target = getScheduleStateFromCache(currentMinutes);

    controlDevice(1, target.fan,   objects.fan);
    controlDevice(2, target.light, objects.light);
    controlDevice(3, target.pump,  objects.pump);
    controlDevice(4, target.spray, objects.spray);
    controlDevice(5, target.reserve, objects.reserve); // <--- [เพิ่ม] ควบคุมสถานะ reserve
    
    // ควบคุม UI Panel อื่นๆ (ถ้ามี)
    controlDevice(1, target.fan,   objects.panel_fan);
    controlDevice(2, target.light, objects.panel_light);
    controlDevice(3, target.pump,  objects.panel_pump);
    controlDevice(4, target.spray, objects.panel_spray);
    controlDevice(5, target.reserve, objects.panel_reserve); // <--- [เพิ่ม] ควบคุมสถานะ panel_reserve
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
inline void loadScreen(enum ScreensEnum screenId) {
    // ล้าง Pointer เก่าทิ้ง เพื่อไม่ให้ Logic เก่าทำงานผิดพลาด
    memset(&objects, 0, sizeof(objects_t)); 

    switch (screenId) {
        case SCREEN_ID_MAIN:
            create_screen_main();
            lv_scr_load_anim(objects.main, LV_SCR_LOAD_ANIM_NONE, 0, 0, true);
            break;
        case SCREEN_ID_PG_WORK:
            create_screen_pg_work();
            lv_scr_load_anim(objects.pg_work, LV_SCR_LOAD_ANIM_NONE, 0, 0, true);
            break;
        case SCREEN_ID_PG_SWITCH:
            create_screen_pg_switch();
            lv_scr_load_anim(objects.pg_switch, LV_SCR_LOAD_ANIM_NONE, 0, 0, true);
            break;
        case SCREEN_ID_SET_TIME:
            create_screen_set_time();
            lv_scr_load_anim(objects.set_time, LV_SCR_LOAD_ANIM_NONE, 0, 0, true);
            break;
        case SCREEN_ID_PG_SENSER:
            create_screen_pg_senser();
            lv_scr_load_anim(objects.pg_senser, LV_SCR_LOAD_ANIM_NONE, 0, 0, true);
            break;
        case SCREEN_ID_PG_TIME:
            create_screen_pg_time();
            lv_scr_load_anim(objects.pg_time, LV_SCR_LOAD_ANIM_NONE, 0, 0, true);
            break;
            
        default: break;
    }
}

#endif