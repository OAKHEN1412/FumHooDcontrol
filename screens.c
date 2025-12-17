#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

#include <string.h>

objects_t objects;
lv_obj_t *tick_value_change_obj;
uint32_t active_theme_index = 0;

void create_screen_main() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.main = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 320);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj0 = obj;
            lv_obj_set_pos(obj, -10, -3);
            lv_obj_set_size(obj, 501, 40);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3162ff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj1 = obj;
                    lv_obj_set_pos(obj, 69, -7);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_34, LV_PART_MAIN | LV_STATE_CHECKED | LV_STATE_PRESSED);
                    lv_label_set_text(obj, "FUME HOOD CONTROL SYSTEM");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 350, 45);
            lv_obj_set_size(obj, 121, 97);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // timer1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.timer1 = obj;
                    lv_obj_set_pos(obj, 10, -1);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff0057cf), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "15:10");
                }
                {
                    // dd_mm_yy_1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.dd_mm_yy_1 = obj;
                    lv_obj_set_pos(obj, 0, 46);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff0057cf), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "21/11/2025");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 351, 154);
            lv_obj_set_size(obj, 121, 147);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.obj2 = obj;
                    lv_obj_set_pos(obj, 5, 46);
                    lv_obj_set_size(obj, 78, 58);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1be400), LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_decor(obj, LV_TEXT_DECOR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "AUTO");
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj3 = obj;
                    lv_obj_set_pos(obj, -6, -1);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "MODE");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 11, 251);
            lv_obj_set_size(obj, 335, 64);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_pad_left(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.obj4 = obj;
                    lv_obj_set_pos(obj, 154, -5);
                    lv_obj_set_size(obj, 115, 37);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1be400), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0303), LV_PART_MAIN | LV_STATE_PRESSED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // m_fan
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.m_fan = obj;
                            lv_obj_set_pos(obj, 0, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "AUTO");
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj5 = obj;
                    lv_obj_set_pos(obj, 6, 2);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff212121), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "MODE FAN :");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 11, 47);
            lv_obj_set_size(obj, 335, 64);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_pad_left(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // SOUND_1
                    lv_obj_t *obj = lv_imgbtn_create(parent_obj);
                    objects.sound_1 = obj;
                    lv_obj_set_pos(obj, 258, -11);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, 50);
                    lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_RELEASED, NULL, &img_sound_open, NULL);
                    lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_PRESSED, NULL, &img_sound_close, NULL);
                    lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_CHECKED_RELEASED, NULL, &img_sound_close, NULL);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_CHECKABLE);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj6 = obj;
                    lv_obj_set_pos(obj, -9, 2);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff212121), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "AirFlow :");
                }
                {
                    // senser
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.senser = obj;
                    lv_obj_set_pos(obj, 98, 3);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff212121), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "000");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 10, 114);
            lv_obj_set_size(obj, 162, 64);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_pad_left(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // fan_1
                    lv_obj_t *obj = lv_switch_create(parent_obj);
                    objects.fan_1 = obj;
                    lv_obj_set_pos(obj, 86, 2);
                    lv_obj_set_size(obj, 50, 25);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0303), LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj7 = obj;
                    lv_obj_set_pos(obj, 1, 2);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff212121), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "FAN :");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 183, 113);
            lv_obj_set_size(obj, 162, 64);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_pad_left(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // light_1
                    lv_obj_t *obj = lv_switch_create(parent_obj);
                    objects.light_1 = obj;
                    lv_obj_set_pos(obj, 86, 2);
                    lv_obj_set_size(obj, 50, 25);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0303), LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj8 = obj;
                    lv_obj_set_pos(obj, -10, 2);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff212121), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "LIGHT :");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 183, 182);
            lv_obj_set_size(obj, 162, 64);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_pad_left(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj9 = obj;
                    lv_obj_set_pos(obj, -10, 3);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff212121), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "SPRAY :");
                }
                {
                    // spray_1
                    lv_obj_t *obj = lv_switch_create(parent_obj);
                    objects.spray_1 = obj;
                    lv_obj_set_pos(obj, 86, 2);
                    lv_obj_set_size(obj, 50, 25);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0303), LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 10, 183);
            lv_obj_set_size(obj, 162, 64);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_pad_left(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj10 = obj;
                    lv_obj_set_pos(obj, -5, 3);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff212121), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "PUMP :");
                }
                {
                    // pump_1
                    lv_obj_t *obj = lv_switch_create(parent_obj);
                    objects.pump_1 = obj;
                    lv_obj_set_pos(obj, 86, 2);
                    lv_obj_set_size(obj, 50, 25);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0303), LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
    }
    
    tick_screen_main();
}

void tick_screen_main() {
}

void create_screen_direac() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.direac = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 320);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj11 = obj;
            lv_obj_set_pos(obj, -10, -3);
            lv_obj_set_size(obj, 501, 40);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3162ff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj12 = obj;
                    lv_obj_set_pos(obj, 69, -7);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_34, LV_PART_MAIN | LV_STATE_CHECKED | LV_STATE_PRESSED);
                    lv_label_set_text(obj, "FUME HOOD CONTROL SYSTEM");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 350, 45);
            lv_obj_set_size(obj, 121, 97);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // timer
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.timer = obj;
                    lv_obj_set_pos(obj, 10, -1);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff0057cf), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "15:10");
                }
                {
                    // dd_mm_yy_2
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.dd_mm_yy_2 = obj;
                    lv_obj_set_pos(obj, 0, 46);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff0057cf), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "21/11/2025");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 351, 154);
            lv_obj_set_size(obj, 121, 147);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.obj13 = obj;
                    lv_obj_set_pos(obj, 5, 46);
                    lv_obj_set_size(obj, 78, 58);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_CHECKABLE);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xfff42727), LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_decor(obj, LV_TEXT_DECOR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "DRC");
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj14 = obj;
                    lv_obj_set_pos(obj, -6, -1);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "MODE");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 11, 251);
            lv_obj_set_size(obj, 335, 64);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_pad_left(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // mode_fan
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.mode_fan = obj;
                    lv_obj_set_pos(obj, 154, -5);
                    lv_obj_set_size(obj, 115, 37);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_CHECKABLE);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1be400), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xfff42727), LV_PART_MAIN | LV_STATE_PRESSED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // mode_fan_label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.mode_fan_label = obj;
                            lv_obj_set_pos(obj, 0, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "AUTO");
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj15 = obj;
                    lv_obj_set_pos(obj, 6, 2);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff212121), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "MODE FAN :");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 10, 45);
            lv_obj_set_size(obj, 335, 64);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_pad_left(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // SOUND_2
                    lv_obj_t *obj = lv_imgbtn_create(parent_obj);
                    objects.sound_2 = obj;
                    lv_obj_set_pos(obj, 259, -11);
                    lv_obj_set_size(obj, 50, 50);
                    lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_RELEASED, NULL, &img_sound_open, NULL);
                    lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_PRESSED, NULL, &img_sound_close, NULL);
                    lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_CHECKED_RELEASED, NULL, &img_sound_close, NULL);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_CHECKABLE|LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj16 = obj;
                    lv_obj_set_pos(obj, -9, 2);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff212121), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "AirFlow :");
                }
                {
                    // senser_1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.senser_1 = obj;
                    lv_obj_set_pos(obj, 98, 3);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff212121), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "000");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 10, 114);
            lv_obj_set_size(obj, 162, 64);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_pad_left(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // fan
                    lv_obj_t *obj = lv_switch_create(parent_obj);
                    objects.fan = obj;
                    lv_obj_set_pos(obj, 86, 2);
                    lv_obj_set_size(obj, 50, 25);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0303), LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj17 = obj;
                    lv_obj_set_pos(obj, 1, 2);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff212121), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "FAN :");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 183, 113);
            lv_obj_set_size(obj, 162, 64);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_pad_left(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // light
                    lv_obj_t *obj = lv_switch_create(parent_obj);
                    objects.light = obj;
                    lv_obj_set_pos(obj, 86, 2);
                    lv_obj_set_size(obj, 50, 25);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0303), LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj18 = obj;
                    lv_obj_set_pos(obj, -8, 3);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff212121), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "LIGHT :");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 183, 182);
            lv_obj_set_size(obj, 162, 64);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_pad_left(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj19 = obj;
                    lv_obj_set_pos(obj, -10, 3);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff212121), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "SPRAY :");
                }
                {
                    // spray
                    lv_obj_t *obj = lv_switch_create(parent_obj);
                    objects.spray = obj;
                    lv_obj_set_pos(obj, 86, 2);
                    lv_obj_set_size(obj, 50, 25);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0303), LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 10, 183);
            lv_obj_set_size(obj, 162, 64);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_pad_left(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj20 = obj;
                    lv_obj_set_pos(obj, -5, 3);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff212121), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "PUMP :");
                }
                {
                    // pump
                    lv_obj_t *obj = lv_switch_create(parent_obj);
                    objects.pump = obj;
                    lv_obj_set_pos(obj, 86, 2);
                    lv_obj_set_size(obj, 50, 25);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0303), LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
    }
    
    tick_screen_direac();
}

void tick_screen_direac() {
}

void create_screen_pg_work() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.pg_work = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 320);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj21 = obj;
            lv_obj_set_pos(obj, -10, -3);
            lv_obj_set_size(obj, 501, 40);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3162ff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj22 = obj;
                    lv_obj_set_pos(obj, 69, -7);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_34, LV_PART_MAIN | LV_STATE_CHECKED | LV_STATE_PRESSED);
                    lv_label_set_text(obj, "FUME HOOD CONTROL SYSTEM");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 16, 60);
            lv_obj_set_size(obj, 449, 235);
        }
        {
            // bm_0
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.bm_0 = obj;
            lv_obj_set_pos(obj, 65, 76);
            lv_obj_set_size(obj, 100, 50);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Sunday ");
                }
            }
        }
        {
            // bm_1
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.bm_1 = obj;
            lv_obj_set_pos(obj, 194, 76);
            lv_obj_set_size(obj, 100, 50);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffd900), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Monday ");
                }
            }
        }
        {
            // bm_2
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.bm_2 = obj;
            lv_obj_set_pos(obj, 315, 76);
            lv_obj_set_size(obj, 100, 50);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff010101), LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff00d9), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Tuesday ");
                }
            }
        }
        {
            // bm_3
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.bm_3 = obj;
            lv_obj_set_pos(obj, 66, 153);
            lv_obj_set_size(obj, 100, 50);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff22cb00), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Wednesday ");
                }
            }
        }
        {
            // bm_4
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.bm_4 = obj;
            lv_obj_set_pos(obj, 195, 153);
            lv_obj_set_size(obj, 100, 50);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff9b26), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Thursday ");
                }
            }
        }
        {
            // bm_5
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.bm_5 = obj;
            lv_obj_set_pos(obj, 316, 153);
            lv_obj_set_size(obj, 100, 50);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff00d1ff), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Friday ");
                }
            }
        }
        {
            // bm_6
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.bm_6 = obj;
            lv_obj_set_pos(obj, 195, 227);
            lv_obj_set_size(obj, 100, 50);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff440082), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Saturday ");
                }
            }
        }
    }
    
    tick_screen_pg_work();
}

void tick_screen_pg_work() {
}

void create_screen_pg_switch() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.pg_switch = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 320);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj23 = obj;
            lv_obj_set_pos(obj, -10, -3);
            lv_obj_set_size(obj, 501, 40);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3162ff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj24 = obj;
                    lv_obj_set_pos(obj, 69, -7);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_34, LV_PART_MAIN | LV_STATE_CHECKED | LV_STATE_PRESSED);
                    lv_label_set_text(obj, "FUME HOOD CONTROL SYSTEM");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 11, 45);
            lv_obj_set_size(obj, 458, 257);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // bm_label6
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.bm_label6 = obj;
                    lv_obj_set_pos(obj, 240, 165);
                    lv_obj_set_size(obj, 187, 50);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff001f38), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff006bff), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff02f4de), LV_PART_MAIN | LV_STATE_PRESSED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // d_label6
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.d_label6 = obj;
                            lv_obj_set_pos(obj, 43, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff3d), LV_PART_MAIN | LV_STATE_PRESSED);
                            lv_label_set_text(obj, "Text");
                        }
                        {
                            // state_swich6
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.state_swich6 = obj;
                            lv_obj_set_pos(obj, 112, 3);
                            lv_obj_set_size(obj, 50, 25);
                        }
                        {
                            // h_label6
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.h_label6 = obj;
                            lv_obj_set_pos(obj, -2, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff3d), LV_PART_MAIN | LV_STATE_PRESSED);
                            lv_label_set_text(obj, "00");
                        }
                    }
                }
                {
                    // bm_label4
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.bm_label4 = obj;
                    lv_obj_set_pos(obj, 240, 99);
                    lv_obj_set_size(obj, 187, 50);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff001f38), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff006bff), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff02f4de), LV_PART_MAIN | LV_STATE_PRESSED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // d_label4
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.d_label4 = obj;
                            lv_obj_set_pos(obj, 43, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff3d), LV_PART_MAIN | LV_STATE_PRESSED);
                            lv_label_set_text(obj, "Text");
                        }
                        {
                            // h_label4
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.h_label4 = obj;
                            lv_obj_set_pos(obj, -2, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff3d), LV_PART_MAIN | LV_STATE_PRESSED);
                            lv_label_set_text(obj, "00");
                        }
                        {
                            // state_swich4
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.state_swich4 = obj;
                            lv_obj_set_pos(obj, 112, 3);
                            lv_obj_set_size(obj, 50, 25);
                        }
                    }
                }
                {
                    // bm_label2
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.bm_label2 = obj;
                    lv_obj_set_pos(obj, 240, 31);
                    lv_obj_set_size(obj, 187, 50);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff001f38), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff006bff), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff02f4de), LV_PART_MAIN | LV_STATE_PRESSED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // d_label2
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.d_label2 = obj;
                            lv_obj_set_pos(obj, 43, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff3d), LV_PART_MAIN | LV_STATE_PRESSED);
                            lv_label_set_text(obj, "Text");
                        }
                        {
                            // h_label2
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.h_label2 = obj;
                            lv_obj_set_pos(obj, -2, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff3d), LV_PART_MAIN | LV_STATE_PRESSED);
                            lv_label_set_text(obj, "00");
                        }
                        {
                            // state_swich2
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.state_swich2 = obj;
                            lv_obj_set_pos(obj, 112, 3);
                            lv_obj_set_size(obj, 50, 25);
                        }
                    }
                }
                {
                    // bm_label5
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.bm_label5 = obj;
                    lv_obj_set_pos(obj, -8, 164);
                    lv_obj_set_size(obj, 191, 50);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff001f38), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff006bff), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff02f4de), LV_PART_MAIN | LV_STATE_PRESSED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // d_label5
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.d_label5 = obj;
                            lv_obj_set_pos(obj, 43, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff3d), LV_PART_MAIN | LV_STATE_PRESSED);
                            lv_label_set_text(obj, "Text");
                        }
                        {
                            // h_label5
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.h_label5 = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff3d), LV_PART_MAIN | LV_STATE_PRESSED);
                            lv_label_set_text(obj, "00");
                        }
                        {
                            // state_swich5
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.state_swich5 = obj;
                            lv_obj_set_pos(obj, 112, 3);
                            lv_obj_set_size(obj, 50, 25);
                        }
                    }
                }
                {
                    // bm_label3
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.bm_label3 = obj;
                    lv_obj_set_pos(obj, -8, 98);
                    lv_obj_set_size(obj, 191, 50);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff001f38), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff006bff), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff02f4de), LV_PART_MAIN | LV_STATE_PRESSED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // d_label3
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.d_label3 = obj;
                            lv_obj_set_pos(obj, 43, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff3d), LV_PART_MAIN | LV_STATE_PRESSED);
                            lv_label_set_text(obj, "Text");
                        }
                        {
                            // h_label3
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.h_label3 = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff3d), LV_PART_MAIN | LV_STATE_PRESSED);
                            lv_label_set_text(obj, "00");
                        }
                        {
                            // state_swich3
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.state_swich3 = obj;
                            lv_obj_set_pos(obj, 112, 3);
                            lv_obj_set_size(obj, 50, 25);
                        }
                    }
                }
                {
                    // bm_label1
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.bm_label1 = obj;
                    lv_obj_set_pos(obj, -8, 30);
                    lv_obj_set_size(obj, 191, 50);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff001f38), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff02f4de), LV_PART_MAIN | LV_STATE_PRESSED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // d_label1
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.d_label1 = obj;
                            lv_obj_set_pos(obj, 43, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff3d), LV_PART_MAIN | LV_STATE_PRESSED);
                            lv_label_set_text(obj, "Text");
                        }
                        {
                            // h_label1
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.h_label1 = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff3d), LV_PART_MAIN | LV_STATE_PRESSED);
                            lv_label_set_text(obj, "00");
                        }
                        {
                            // state_swich1
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.state_swich1 = obj;
                            lv_obj_set_pos(obj, 112, 3);
                            lv_obj_set_size(obj, 50, 25);
                        }
                    }
                }
            }
        }
        {
            // pg_switch_day
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.pg_switch_day = obj;
            lv_obj_set_pos(obj, 21, 53);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Text");
        }
    }
    
    tick_screen_pg_switch();
}

void tick_screen_pg_switch() {
}

void create_screen_pg_time() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.pg_time = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 320);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    
    tick_screen_pg_time();
}

void tick_screen_pg_time() {
}

void create_screen_pg_senser() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.pg_senser = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 320);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    
    tick_screen_pg_senser();
}

void tick_screen_pg_senser() {
}

void create_screen_set_time() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.set_time = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 320);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj25 = obj;
            lv_obj_set_pos(obj, -10, -21);
            lv_obj_set_size(obj, 112, 362);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff111744), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // num_save
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.num_save = obj;
                    lv_obj_set_pos(obj, 22, 158);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "0");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj26 = obj;
                    lv_obj_set_pos(obj, 6, 232);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "SAVE");
                }
                {
                    // pg_switch_day_1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.pg_switch_day_1 = obj;
                    lv_obj_set_pos(obj, 13, 66);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Text");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj27 = obj;
            lv_obj_set_pos(obj, -10, -3);
            lv_obj_set_size(obj, 501, 40);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3162ff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xffe0e0e0), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj28 = obj;
                    lv_obj_set_pos(obj, 69, -7);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_34, LV_PART_MAIN | LV_STATE_CHECKED | LV_STATE_PRESSED);
                    lv_label_set_text(obj, "FUME HOOD CONTROL SYSTEM");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 111, 50);
            lv_obj_set_size(obj, 173, 85);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 105, 8);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, ": H");
                }
                {
                    // hour_setting
                    lv_obj_t *obj = lv_dropdown_create(parent_obj);
                    objects.hour_setting = obj;
                    lv_obj_set_pos(obj, -4, 2);
                    lv_obj_set_size(obj, 99, LV_SIZE_CONTENT);
                    lv_dropdown_set_options(obj, "Hour\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n0");
                    lv_dropdown_set_selected(obj, 0);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff006bff), LV_PART_MAIN | LV_STATE_PRESSED);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 105, -14);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "START");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 110, 139);
            lv_obj_set_size(obj, 173, 85);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 113, -14);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "END");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 105, 8);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, ": H");
                }
                {
                    // hour_end
                    lv_obj_t *obj = lv_dropdown_create(parent_obj);
                    objects.hour_end = obj;
                    lv_obj_set_pos(obj, -4, 2);
                    lv_obj_set_size(obj, 99, LV_SIZE_CONTENT);
                    lv_dropdown_set_options(obj, "Hour\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n0");
                    lv_dropdown_set_selected(obj, 0);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff006bff), LV_PART_MAIN | LV_STATE_PRESSED);
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 293, 140);
            lv_obj_set_size(obj, 167, 85);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // min_end
                    lv_obj_t *obj = lv_dropdown_create(parent_obj);
                    objects.min_end = obj;
                    lv_obj_set_pos(obj, -7, 2);
                    lv_obj_set_size(obj, 88, LV_SIZE_CONTENT);
                    lv_dropdown_set_options(obj, "Min\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n51\n52\n53\n54\n55\n56\n57\n58\n59\n60");
                    lv_dropdown_set_selected(obj, 0);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff006bff), LV_PART_MAIN | LV_STATE_PRESSED);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 92, 8);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, ": M");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 106, -14);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "END");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 292, 50);
            lv_obj_set_size(obj, 167, 85);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // min_setting
                    lv_obj_t *obj = lv_dropdown_create(parent_obj);
                    objects.min_setting = obj;
                    lv_obj_set_pos(obj, -7, 2);
                    lv_obj_set_size(obj, 88, LV_SIZE_CONTENT);
                    lv_dropdown_set_options(obj, "Min\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n51\n52\n53\n54\n55\n56\n57\n58\n59\n60");
                    lv_dropdown_set_selected(obj, 0);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff006bff), LV_PART_MAIN | LV_STATE_PRESSED);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 92, 8);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, ": M");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 99, -14);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "START");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 110, 229);
            lv_obj_set_size(obj, 349, 78);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // bt_save
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.bt_save = obj;
                    lv_obj_set_pos(obj, 214, -3);
                    lv_obj_set_size(obj, 100, 50);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "SAVE");
                        }
                    }
                }
                {
                    // device_settting
                    lv_obj_t *obj = lv_dropdown_create(parent_obj);
                    objects.device_settting = obj;
                    lv_obj_set_pos(obj, -4, -1);
                    lv_obj_set_size(obj, 200, LV_SIZE_CONTENT);
                    lv_dropdown_set_options(obj, "Device\nFan\nLight\nPump\nSpray");
                    lv_dropdown_set_selected(obj, 0);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff006bff), LV_PART_MAIN | LV_STATE_PRESSED);
                }
            }
        }
    }
    
    tick_screen_set_time();
}

void tick_screen_set_time() {
}



typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main,
    tick_screen_direac,
    tick_screen_pg_work,
    tick_screen_pg_switch,
    tick_screen_pg_time,
    tick_screen_pg_senser,
    tick_screen_set_time,
};
void tick_screen(int screen_index) {
    tick_screen_funcs[screen_index]();
}
void tick_screen_by_id(enum ScreensEnum screenId) {
    tick_screen_funcs[screenId - 1]();
}

void create_screens() {
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    
    create_screen_main();
    create_screen_direac();
    create_screen_pg_work();
    create_screen_pg_switch();
    create_screen_pg_time();
    create_screen_pg_senser();
    create_screen_set_time();
}
