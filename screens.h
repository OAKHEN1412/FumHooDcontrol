#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *direac;
    lv_obj_t *pg_work;
    lv_obj_t *pg_switch;
    lv_obj_t *pg_time;
    lv_obj_t *pg_senser;
    lv_obj_t *set_time;
    lv_obj_t *obj0;
    lv_obj_t *obj1;
    lv_obj_t *timer1;
    lv_obj_t *dd_mm_yy_1;
    lv_obj_t *obj2;
    lv_obj_t *obj3;
    lv_obj_t *obj4;
    lv_obj_t *m_fan;
    lv_obj_t *obj5;
    lv_obj_t *sound_1;
    lv_obj_t *obj6;
    lv_obj_t *senser;
    lv_obj_t *fan_1;
    lv_obj_t *obj7;
    lv_obj_t *light_1;
    lv_obj_t *obj8;
    lv_obj_t *obj9;
    lv_obj_t *spray_1;
    lv_obj_t *obj10;
    lv_obj_t *pump_1;
    lv_obj_t *obj11;
    lv_obj_t *obj12;
    lv_obj_t *timer;
    lv_obj_t *dd_mm_yy_2;
    lv_obj_t *obj13;
    lv_obj_t *obj14;
    lv_obj_t *mode_fan;
    lv_obj_t *mode_fan_label;
    lv_obj_t *obj15;
    lv_obj_t *sound_2;
    lv_obj_t *obj16;
    lv_obj_t *senser_1;
    lv_obj_t *fan;
    lv_obj_t *obj17;
    lv_obj_t *light;
    lv_obj_t *obj18;
    lv_obj_t *obj19;
    lv_obj_t *spray;
    lv_obj_t *obj20;
    lv_obj_t *pump;
    lv_obj_t *obj21;
    lv_obj_t *obj22;
    lv_obj_t *bm_0;
    lv_obj_t *bm_1;
    lv_obj_t *bm_2;
    lv_obj_t *bm_3;
    lv_obj_t *bm_4;
    lv_obj_t *bm_5;
    lv_obj_t *bm_6;
    lv_obj_t *obj23;
    lv_obj_t *obj24;
    lv_obj_t *bm_label6;
    lv_obj_t *d_label6;
    lv_obj_t *state_swich6;
    lv_obj_t *h_label6;
    lv_obj_t *bm_label4;
    lv_obj_t *d_label4;
    lv_obj_t *h_label4;
    lv_obj_t *state_swich4;
    lv_obj_t *bm_label2;
    lv_obj_t *d_label2;
    lv_obj_t *h_label2;
    lv_obj_t *state_swich2;
    lv_obj_t *bm_label5;
    lv_obj_t *d_label5;
    lv_obj_t *h_label5;
    lv_obj_t *state_swich5;
    lv_obj_t *bm_label3;
    lv_obj_t *d_label3;
    lv_obj_t *h_label3;
    lv_obj_t *state_swich3;
    lv_obj_t *bm_label1;
    lv_obj_t *d_label1;
    lv_obj_t *h_label1;
    lv_obj_t *state_swich1;
    lv_obj_t *pg_switch_day;
    lv_obj_t *obj25;
    lv_obj_t *num_save;
    lv_obj_t *obj26;
    lv_obj_t *pg_switch_day_1;
    lv_obj_t *obj27;
    lv_obj_t *obj28;
    lv_obj_t *hour_setting;
    lv_obj_t *hour_end;
    lv_obj_t *min_end;
    lv_obj_t *min_setting;
    lv_obj_t *bt_save;
    lv_obj_t *device_settting;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN = 1,
    SCREEN_ID_DIREAC = 2,
    SCREEN_ID_PG_WORK = 3,
    SCREEN_ID_PG_SWITCH = 4,
    SCREEN_ID_PG_TIME = 5,
    SCREEN_ID_PG_SENSER = 6,
    SCREEN_ID_SET_TIME = 7,
};

void create_screen_main();
void tick_screen_main();

void create_screen_direac();
void tick_screen_direac();

void create_screen_pg_work();
void tick_screen_pg_work();

void create_screen_pg_switch();
void tick_screen_pg_switch();

void create_screen_pg_time();
void tick_screen_pg_time();

void create_screen_pg_senser();
void tick_screen_pg_senser();

void create_screen_set_time();
void tick_screen_set_time();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/