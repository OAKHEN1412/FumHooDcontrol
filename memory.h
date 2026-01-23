#ifndef MEMORY_H
#define MEMORY_H

#include <Arduino.h>
#include <Preferences.h>
extern Preferences prefs;

struct ProgramData {
    int hour_start[6];    // เปลี่ยนจาก hour เป็น hour_start
    int minute_start[6];  // เปลี่ยนจาก minute เป็น minute_start
    int hour_end[6];      // เพิ่ม hour_end
    int minute_end[6];    // เพิ่ม minute_end
    int fan[6];
    int light[6];
    int spray[6];
    int pump[6];
    int state[6];         // เพิ่ม state
};
void initMemory();
void closeMemory();
void saveTimeSettings(int d, int m, int y, int h, int mn);
void loadTimeSettings(int* d, int* m, int* y, int* h, int* mn);
// void saveProgram(int index, const ProgramData &data);
void saveSingleSlot(int dayIndex, int slotIndex, const ProgramData &data);
void saveLow(float _rLow, float _nLow);
void saveHigh(float _rHigh, float _nHigh);
void loadCalibration();
void loadAlertSetting();
void saveAlertSetting(float value);
void createMockProgramData();
void systemFirstBootCheck();
ProgramData loadProgram(int index);

#endif