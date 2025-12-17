#include "memory.h"

Preferences prefs;

/* ---------------- INIT MEMORY ---------------- */
void initMemory() {
    // เรียกครั้งเดียวตอน start เพื่อเตรียม NVS
    prefs.begin("mydata", false);
    prefs.end();
}

/* ---------------- SAVE PROGRAM ---------------- */
void saveProgram(int index, const ProgramData &data) {
    prefs.begin("mydata", false);

    String keyBase = "p" + String(index);

    for(int i = 0; i < 6; i++){
        // บันทึก Day และ Device เหมือนเดิม
        prefs.putInt((keyBase + "_day" + String(i)).c_str(), data.day[i]);
        prefs.putInt((keyBase + "_dev" + String(i)).c_str(), data.device[i]);

        // --- ส่วนที่แก้ไขและเพิ่มใหม่ ---
        
        // Start Time (เปลี่ยนจาก _hour/_min เป็น _hs/_ms เพื่อประหยัดพื้นที่ Key)
        prefs.putInt((keyBase + "_hs" + String(i)).c_str(), data.hour_start[i]);
        prefs.putInt((keyBase + "_ms" + String(i)).c_str(), data.minute_start[i]);

        // End Time (เพิ่มใหม่)
        prefs.putInt((keyBase + "_he" + String(i)).c_str(), data.hour_end[i]);
        prefs.putInt((keyBase + "_me" + String(i)).c_str(), data.minute_end[i]);

        // State (เพิ่มใหม่)
        prefs.putInt((keyBase + "_st" + String(i)).c_str(), data.state[i]);
    }

    prefs.end();
}

/* ---------------- LOAD PROGRAM ---------------- */
ProgramData loadProgram(int index) {
    prefs.begin("mydata", true); // true = read only

    String keyBase = "p" + String(index);
    ProgramData data;

    for(int i = 0; i < 6; i++){
        // โหลด Day และ Device
        data.day[i]    = prefs.getInt((keyBase + "_day" + String(i)).c_str(), 0);
        data.device[i] = prefs.getInt((keyBase + "_dev" + String(i)).c_str(), 0);

        // --- ส่วนที่แก้ไขและเพิ่มใหม่ ---

        // Start Time
        data.hour_start[i]   = prefs.getInt((keyBase + "_hs" + String(i)).c_str(), 0);
        data.minute_start[i] = prefs.getInt((keyBase + "_ms" + String(i)).c_str(), 0);

        // End Time
        data.hour_end[i]     = prefs.getInt((keyBase + "_he" + String(i)).c_str(), 0);
        data.minute_end[i]   = prefs.getInt((keyBase + "_me" + String(i)).c_str(), 0);

        // State
        data.state[i]        = prefs.getInt((keyBase + "_st" + String(i)).c_str(), 0); // Default 0 (OFF)
    }

    prefs.end();
    return data;
}