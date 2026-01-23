#include <Arduino.h>
#include <Preferences.h>

// ==========================================
// ส่วนที่เพิ่มเข้ามาแทนไฟล์ Constants.h
// ==========================================

// โครงสร้างข้อมูลสำหรับเก็บค่าการตั้งเวลา
struct ProgramData {
    int hour_start[6];
    int minute_start[6];
    int hour_end[6];
    int minute_end[6];
    
    // แยกเก็บสถานะของอุปกรณ์ 4 ชนิด (0=ปิด, 1=เปิด)
    int fan[6];
    int light[6];
    int pump[6];
    int spray[6];
    
    // สถานะเปิด/ปิดการทำงานของ Slot นั้นๆ
    int state[6];
};

// สร้าง Object สำหรับการบันทึกข้อมูล
Preferences prefs;

// ==========================================
// ฟังก์ชันบันทึกและโหลดข้อมูล
// ==========================================

// ฟังก์ชันบันทึกข้อมูลทีละ Slot
// รับค่าแบบ const ProgramData &data เพื่อความถูกต้องและประหยัด RAM
void saveSingleSlot(int dayIndex, int slotIndex, const ProgramData &data) {
    prefs.begin("mydata", false); // เปิดโหมด Read/Write

    String keyBase = "p" + String(dayIndex);
    String s = String(slotIndex); // ตัวแปร s สำหรับแปลงเลข Slot เป็น String

    // บันทึกแยก 4 อุปกรณ์
    prefs.putInt((keyBase + "_fan"   + s).c_str(), data.fan[slotIndex]);
    prefs.putInt((keyBase + "_light" + s).c_str(), data.light[slotIndex]);
    prefs.putInt((keyBase + "_spray" + s).c_str(), data.spray[slotIndex]);
    prefs.putInt((keyBase + "_pump"  + s).c_str(), data.pump[slotIndex]);

    // บันทึกเวลาและสถานะหลัก
    prefs.putInt((keyBase + "_hs"  + s).c_str(), data.hour_start[slotIndex]);
    prefs.putInt((keyBase + "_ms"  + s).c_str(), data.minute_start[slotIndex]);
    prefs.putInt((keyBase + "_he"  + s).c_str(), data.hour_end[slotIndex]);
    prefs.putInt((keyBase + "_me"  + s).c_str(), data.minute_end[slotIndex]);
    prefs.putInt((keyBase + "_st"  + s).c_str(), data.state[slotIndex]);

    prefs.end();
}

// ฟังก์ชันโหลดข้อมูลทั้งหมดของวันนั้นๆ
ProgramData loadProgram(int index) {
    ProgramData data;
    prefs.begin("mydata", true); // เปิดโหมด Read Only
    
    String keyBase = "p" + String(index);

    for (int i = 0; i < 6; i++) {
        String s = String(i); // ตัวแปร s สำหรับวนลูป

        // อ่านค่าอุปกรณ์ (ถ้าไม่มีให้ค่าเริ่มต้นเป็น 0)
        data.fan[i]   = prefs.getInt((keyBase + "_fan"   + s).c_str(), 0);
        data.light[i] = prefs.getInt((keyBase + "_light" + s).c_str(), 0);
        data.spray[i] = prefs.getInt((keyBase + "_spray" + s).c_str(), 0);
        data.pump[i]  = prefs.getInt((keyBase + "_pump"  + s).c_str(), 0);
        
        // อ่านค่าเวลา
        data.hour_start[i]   = prefs.getInt((keyBase + "_hs" + s).c_str(), 0);
        data.minute_start[i] = prefs.getInt((keyBase + "_ms" + s).c_str(), 0);
        data.hour_end[i]     = prefs.getInt((keyBase + "_he" + s).c_str(), 0);
        data.minute_end[i]   = prefs.getInt((keyBase + "_me" + s).c_str(), 0);
        data.state[i]        = prefs.getInt((keyBase + "_st" + s).c_str(), 0);
    }
    prefs.end();
    return data;
}
void initMemory() {
    // ฟังก์ชันนี้เอาไว้เริ่มระบบ (ถ้าจำเป็น)
    // แต่เนื่องจากเราสั่ง prefs.begin ใน save/load แล้ว
    // ตรงนี้ปล่อยว่างไว้ หรือสั่ง print เช็คสถานะก็ได้ครับ
    Serial.println("Memory Control: Ready");
}

// --- เพิ่มต่อท้ายในไฟล์ memory.cpp ---

// ต้องประกาศตัวแปร rtc เพื่อเรียกใช้ในไฟล์นี้
#include <RTClib.h> 
extern RTC_DS3231 rtc; 

void saveTimeSettings(int d, int m, int y, int h, int mn) {
    Serial.println(F("Process: Saving to Flash..."));
    
    // 1. เขียนลง Preferences
    if (prefs.begin("sys_time", false)) {
        prefs.putInt("dd", d);
        prefs.putInt("mm", m);
        prefs.putInt("yy", y);
        prefs.putInt("hh", h);
        prefs.putInt("mn", mn);
        prefs.end();
    }
    
    // พักให้ Flash ทำงานเสร็จ
    vTaskDelay(20 / portTICK_PERIOD_MS);

    // 2. เขียนลง RTC (ป้องกันการค้างด้วยการล้าง Bus)
    Serial.println(F("Process: Adjusting RTC..."));
    Wire.beginTransmission(0x68); // ที่อยู่ RTC ทั่วไป
    if (Wire.endTransmission() != 0) {
        Serial.println(F("I2C Error: Resetting Wire..."));
        Wire.begin(); // Reset บัสถ้าติดต่อไม่ได้
    }
    
    rtc.adjust(DateTime(y, m, d, h, mn, 0));
    Serial.println(F("Success: All Saved."));
}

// ฟังก์ชันโหลดค่าเวลาล่าสุดที่เคยตั้งไว้ (เอาไว้ใส่กลับใน Dropdown ตอนเปิดหน้า)
void loadTimeSettings(int* d, int* m, int* y, int* h, int* mn) {
    prefs.begin("sys_time", true); // Read-only
    *d  = prefs.getInt("dd", 1);   // ค่า Default วันที่ 1
    *m  = prefs.getInt("mm", 1);   // ค่า Default เดือน 1
    *y  = prefs.getInt("yy", 2024);// ค่า Default ปี 2024
    *h  = prefs.getInt("hh", 12);
    *mn = prefs.getInt("mn", 0);
    prefs.end();
}


float rLow = 9699.95, rHigh = 13173.06;
float nLow = 0.60,    nHigh = 2.00;
// --- ฟังก์ชันที่ 1: บันทึกค่า Low ---
void saveLow(float _rLow, float _nLow) {
    if (prefs.begin("calib", false)) {
        rLow = _rLow; // อัปเดตตัวแปรใน RAM
        nLow = _nLow;
        prefs.putFloat("r_low", rLow);
        prefs.putFloat("n_low", nLow);
        prefs.end();
        Serial2.printf("[Saved Low] Raw: %.2f, Ref: %.2f\n", rLow, nLow);
    } else {
        Serial2.println("Error: Failed to open NVS");
    }
}

// --- ฟังก์ชันที่ 2: บันทึกค่า High ---
void saveHigh(float _rHigh, float _nHigh) {
    if (prefs.begin("calib", false)) {
        rHigh = _rHigh; // อัปเดตตัวแปรใน RAM
        nHigh = _nHigh;
        prefs.putFloat("r_high", rHigh);
        prefs.putFloat("n_high", nHigh);
        prefs.end();
        Serial2.printf("[Saved High] Raw: %.2f, Ref: %.2f\n", rHigh, nHigh);
    } else {
        Serial2.println("Error: Failed to open NVS");
    }
}

void loadCalibration() {
    if (prefs.begin("calib", true)) { 
        // แก้ไข: ให้ nLow เริ่มที่ 0.00 เพื่อให้ตรงกับจุด Zero
        rLow  = prefs.getFloat("r_low", 9699.95);
        nLow  = prefs.getFloat("n_low", 0.00);    // เปลี่ยนจาก 0.60 เป็น 0.00
        rHigh = prefs.getFloat("r_high", 13173.06);
        nHigh = prefs.getFloat("n_high", 2.00);
        prefs.end();
        Serial2.println("[System] Calibration Data Loaded.");
    }
}


// ฟังก์ชันบันทึกค่า Alert ลงในหน่วยความจำ
void saveAlertSetting(float value) {
    if (prefs.begin("settings", false)) { // ใช้ Namespace "settings" แยกจาก "calib"
        prefs.putFloat("alert_val", value);
        prefs.end();
        Serial2.printf("[System] Alert Set Saved: %.2f\n", value);
        
        // แสดงการแจ้งเตือนบนหน้าจอ (ถ้าคุณมีฟังก์ชัน showSaveAlert จากข้อที่แล้ว)
        // showSaveAlert("Settings", "Alert Value Saved!");
    } else {
        Serial2.println("[Error] Cannot open NVS for settings");
    }
}
float alert_set;
// ฟังก์ชันโหลดค่า Alert (เรียกใน setup)
void loadAlertSetting() {
    if (prefs.begin("settings", true)) {
        // อ่านค่า alert_val ถ้าไม่มีให้ใช้ค่า Default เป็น 0.50
        alert_set = prefs.getFloat("alert_val", 1.0); 
        prefs.end();
        Serial2.printf("[System] Alert Loaded: %.2f\n", alert_set);
    }
}




void createMockProgramData() {
    Serial.println("[NVS] Generating Mock Data for 7 Days...");

    for (int day = 0; day < 7; day++) { // วนลูป 7 วัน (อาทิตย์ - เสาร์)
        ProgramData mock;
        
        for (int slot = 0; slot < 6; slot++) { // วนลูป 6 ช่วงเวลาต่อวัน
            // จำลองเวลา: Slot 0 เริ่ม 8 โมง, Slot 1 เริ่ม 10 โมง...
            mock.hour_start[slot]   = 8 + (slot * 2); 
            mock.minute_start[slot] = 0;
            mock.hour_end[slot]     = 9 + (slot * 2);
            mock.minute_end[slot]   = 30;

            // จำลองสถานะอุปกรณ์ (เปิดสลับปิดให้พอเห็นผลทดสอบ)
            mock.fan[slot]   = (slot % 2 == 0) ? 1 : 0; // Slot คู่เปิดพัดลม
            mock.light[slot] = 1;                       // เปิดไฟทุก Slot
            mock.pump[slot]  = 0;
            mock.spray[slot] = 0;
            
            mock.state[slot] = 1; // เปิดใช้งาน Slot นี้

            // บันทึกลง NVS โดยใช้ฟังก์ชันเดิมของคุณ
            saveSingleSlot(day, slot, mock);
        }
        Serial.printf("Day %d initialized...\n", day);
        delay(10); // ให้เวลา CPU พักหายใจ (กัน Watchdog Trigger)
    }
}




void systemFirstBootCheck() {
    prefs.begin("system", false);
    
    // ตรวจสอบคีย์ "is_ready" ถ้าไม่มีค่า (พึ่งเปิดครั้งแรก) จะได้ false
    bool isReady = prefs.getBool("is_ready", false);

    if (!isReady) {
        Serial.println(">>> FIRST BOOT DETECTED! Initializing NVS...");

        // 1. สร้างข้อมูลตารางงานจำลอง
        createMockProgramData();

        // 2. ตั้งค่า Calibration เริ่มต้น
        saveLow(9699.95, 0.00);
        saveHigh(13173.06, 2.00);

        // 3. ตั้งค่า Alert เริ่มต้น
        saveAlertSetting(1.0);

        // 4. บันทึกสถานะว่าระบบพร้อมแล้ว
        prefs.putBool("is_ready", true);
        Serial.println(">>> NVS INITIALIZATION COMPLETE!");
    } else {
        Serial.println("[System] NVS already contains data. Ready to go.");
    }
    
    prefs.end();
}