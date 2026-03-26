#ifndef MEMORY_H
#define MEMORY_H

#include <Arduino.h>
#include <Preferences.h>

extern Preferences prefs;

// ==========================================
// ตัวแปร Global (เพื่อให้ไฟล์อื่นดึงค่าไปใช้ได้)
// ==========================================
extern float rLow, rHigh;
extern float nLow, nHigh;
extern float alert_set;

// ==========================================
// โครงสร้างข้อมูลการตั้งเวลา
// ==========================================
struct ProgramData {
    int hour_start[6];    // เวลาเริ่ม (ชั่วโมง)
    int minute_start[6];  // เวลาเริ่ม (นาที)
    int hour_end[6];      // เวลาจบ (ชั่วโมง)
    int minute_end[6];    // เวลาจบ (นาที)
    
    // สถานะอุปกรณ์
    int fan[6];
    int light[6];
    int pump[6];
    int spray[6];
    int reserve[6];       // <--- เพิ่ม reserve 
    
    int state[6];         // สถานะเปิด/ปิดของ Slot
};

// ==========================================
// ฟังก์ชันจัดการหน่วยความจำและการตั้งค่า
// ==========================================

// การจัดการทั่วไป
void initMemory();
void closeMemory();

// การจัดการตารางเวลา
ProgramData loadProgram(int index);
void saveSingleSlot(int dayIndex, int slotIndex, const ProgramData &data);
void createMockProgramData();
void systemFirstBootCheck();

// การจัดการ RTC Time
void saveTimeSettings(int d, int m, int y, int h, int mn);
void loadTimeSettings(int* d, int* m, int* y, int* h, int* mn);

// การจัดการ Calibration
void saveLow(float _rLow, float _nLow);
void saveHigh(float _rHigh, float _nHigh);
void loadCalibration();

// การจัดการ Alert
void saveAlertSetting(float value);
void loadAlertSetting();

#endif