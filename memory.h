#ifndef MEMORY_H
#define MEMORY_H

#include <Arduino.h>
#include <Preferences.h>

struct ProgramData {
    int day[6];
    int hour_start[6];    // เปลี่ยนจาก hour เป็น hour_start
    int minute_start[6];  // เปลี่ยนจาก minute เป็น minute_start
    int hour_end[6];      // เพิ่ม hour_end
    int minute_end[6];    // เพิ่ม minute_end
    int device[6];
    int state[6];         // เพิ่ม state
};

void initMemory();
void saveProgram(int index, const ProgramData &data);
ProgramData loadProgram(int index);

#endif