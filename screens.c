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
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff121921), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj0 = obj;
            lv_obj_set_pos(obj, 352, 0);
            lv_obj_set_size(obj, 128, 320);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff121921), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff0002c0), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj1 = obj;
                    lv_obj_set_pos(obj, 1, -7);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &ui_font_thai_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "MODE & TIME");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj2 = obj;
                    lv_obj_set_pos(obj, 0, 42);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &ui_font_thai_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "MODE");
                }
                {
                    // mode
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.mode = obj;
                    lv_obj_set_pos(obj, -11, 66);
                    lv_obj_set_size(obj, 100, 43);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1be400), LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // mode_label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.mode_label = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &ui_font_thai_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_decor(obj, LV_TEXT_DECOR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "AUTO");
                        }
                    }
                }
                {
                    // timer1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.timer1 = obj;
                    lv_obj_set_pos(obj, 1, 94);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &ui_font_thai_30, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "15:10");
                }
                {
                    // dd_mm_yy_1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.dd_mm_yy_1 = obj;
                    lv_obj_set_pos(obj, 1, 130);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &ui_font_thai_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffe0e0e0), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "21/11/2025");
                }
                {
                    // speed
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.speed = obj;
                    lv_obj_set_pos(obj, -11, 141);
                    lv_obj_set_size(obj, 100, 43);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1be400), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0303), LV_PART_MAIN | LV_STATE_PRESSED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // m_fan
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.m_fan = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &ui_font_thai_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "AUTO");
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj3 = obj;
                    lv_obj_set_pos(obj, 1, 118);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &ui_font_thai_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "MODE FAN");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj4 = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 352, 97);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_pad_left(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff121921), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.obj5 = obj;
                    lv_obj_set_pos(obj, 127, 6);
                    lv_obj_set_size(obj, 119, 45);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff22262b), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff2fabff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_FULL, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // senser
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.senser = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff2fabff), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "000 M/s");
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 253, 3);
                    lv_obj_set_size(obj, 58, 51);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // SOUND_1
                            lv_obj_t *obj = lv_imgbtn_create(parent_obj);
                            objects.sound_1 = obj;
                            lv_obj_set_pos(obj, -14, -17);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, 50);
                            lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_RELEASED, NULL, &img_sound_open, NULL);
                            lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_PRESSED, NULL, &img_sound_close, NULL);
                            lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_CHECKED_RELEASED, NULL, &img_sound_close, NULL);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_CHECKABLE);
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj6 = obj;
                    lv_obj_set_pos(obj, -3, 9);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_thai_30, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Air Flow");
                }
            }
        }
        {
            // panel_fan
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.panel_fan = obj;
            lv_obj_set_pos(obj, 0, 97);
            lv_obj_set_size(obj, 176, 75);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_pad_left(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff121921), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff1300ff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1300ff), LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff1300ff), LV_PART_MAIN | LV_STATE_CHECKED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // fan
                    lv_obj_t *obj = lv_switch_create(parent_obj);
                    objects.fan = obj;
                    lv_obj_set_pos(obj, 101, 5);
                    lv_obj_set_size(obj, 43, 25);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0303), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff00cc28), LV_PART_INDICATOR | LV_STATE_CHECKED);
                }
                {
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    lv_obj_set_pos(obj, -20, -7);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_img_set_src(obj, &img_fan);
                    lv_img_set_angle(obj, 230);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj7 = obj;
                    lv_obj_set_pos(obj, 45, 2);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_thai_22, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "FAN");
                }
            }
        }
        {
            // panel_light
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.panel_light = obj;
            lv_obj_set_pos(obj, 176, 97);
            lv_obj_set_size(obj, 176, 75);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_pad_left(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff121921), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff1300ff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1300ff), LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff1300ff), LV_PART_MAIN | LV_STATE_CHECKED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // light
                    lv_obj_t *obj = lv_switch_create(parent_obj);
                    objects.light = obj;
                    lv_obj_set_pos(obj, 99, 5);
                    lv_obj_set_size(obj, 43, 25);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0303), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff00cc28), LV_PART_INDICATOR | LV_STATE_CHECKED);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj8 = obj;
                    lv_obj_set_pos(obj, 28, 2);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_thai_22, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "LIGHT");
                }
                {
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    lv_obj_set_pos(obj, -20, -7);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_img_set_src(obj, &img_light);
                }
            }
        }
        {
            // panel_reserve_1
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.panel_reserve_1 = obj;
            lv_obj_set_pos(obj, 0, 245);
            lv_obj_set_size(obj, 176, 75);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_pad_left(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff121921), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff1300ff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff2fabff), LV_PART_MAIN | LV_STATE_CHECKED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // alram_3
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.alram_3 = obj;
                    lv_obj_set_pos(obj, 73, 24);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_opa(obj, 255, LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_opa(obj, 100, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Alram_3");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj9 = obj;
                    lv_obj_set_pos(obj, -6, -2);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_thai_22, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Alarm :");
                }
                {
                    // alram_2
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.alram_2 = obj;
                    lv_obj_set_pos(obj, 73, 5);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_opa(obj, 255, LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_opa(obj, 100, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Alram_2");
                }
                {
                    // alram_1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.alram_1 = obj;
                    lv_obj_set_pos(obj, 75, -13);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_opa(obj, 255, LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_opa(obj, 100, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Alram_1");
                }
            }
        }
        {
            // panel_reserve
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.panel_reserve = obj;
            lv_obj_set_pos(obj, 176, 245);
            lv_obj_set_size(obj, 176, 75);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_pad_left(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff121921), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff1300ff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1300ff), LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff1300ff), LV_PART_MAIN | LV_STATE_CHECKED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj10 = obj;
                    lv_obj_set_pos(obj, -4, 2);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_thai_22, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "RESERVE");
                }
                {
                    // reserve
                    lv_obj_t *obj = lv_switch_create(parent_obj);
                    objects.reserve = obj;
                    lv_obj_set_pos(obj, 98, 5);
                    lv_obj_set_size(obj, 43, 25);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0303), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff00cc28), LV_PART_INDICATOR | LV_STATE_CHECKED);
                }
            }
        }
        {
            // panel_spray
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.panel_spray = obj;
            lv_obj_set_pos(obj, 176, 170);
            lv_obj_set_size(obj, 176, 75);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_pad_left(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff121921), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff1300ff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1300ff), LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff1300ff), LV_PART_MAIN | LV_STATE_CHECKED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj11 = obj;
                    lv_obj_set_pos(obj, 26, 2);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_thai_22, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "SPRAY");
                }
                {
                    // spray
                    lv_obj_t *obj = lv_switch_create(parent_obj);
                    objects.spray = obj;
                    lv_obj_set_pos(obj, 99, 5);
                    lv_obj_set_size(obj, 43, 25);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0303), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff00cc28), LV_PART_INDICATOR | LV_STATE_CHECKED);
                }
                {
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    lv_obj_set_pos(obj, -20, -7);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_img_set_src(obj, &img_spray);
                }
            }
        }
        {
            // panel_pump
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.panel_pump = obj;
            lv_obj_set_pos(obj, 0, 172);
            lv_obj_set_size(obj, 176, 75);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_pad_left(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff121921), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff1300ff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1300ff), LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff1300ff), LV_PART_MAIN | LV_STATE_CHECKED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj12 = obj;
                    lv_obj_set_pos(obj, 35, 2);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_thai_22, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "PUMP");
                }
                {
                    // pump
                    lv_obj_t *obj = lv_switch_create(parent_obj);
                    objects.pump = obj;
                    lv_obj_set_pos(obj, 101, 5);
                    lv_obj_set_size(obj, 43, 25);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0303), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff00cc28), LV_PART_INDICATOR | LV_STATE_CHECKED);
                }
                {
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    lv_obj_set_pos(obj, -20, -7);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_img_set_src(obj, &img_pump);
                }
            }
        }
    }
    
    tick_screen_main();
}

void tick_screen_main() {
}

void create_screen_pg_work() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.pg_work = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 320);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff121921), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj13 = obj;
            lv_obj_set_pos(obj, 26, 45);
            lv_obj_set_size(obj, 427, 231);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff2fabff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000b2e), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // bm_0
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.bm_0 = obj;
            lv_obj_set_pos(obj, 59, 60);
            lv_obj_set_size(obj, 112, 50);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000b2e), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // bmt_0
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.bmt_0 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &ui_font_thai_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_label_set_text(obj, "SUN");
                }
            }
        }
        {
            // bm_1
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.bm_1 = obj;
            lv_obj_set_pos(obj, 184, 60);
            lv_obj_set_size(obj, 112, 50);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000b2e), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xffffd900), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffd900), LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // bmt_1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.bmt_1 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffd900), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_thai_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_label_set_text(obj, "MON");
                }
            }
        }
        {
            // bm_2
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.bm_2 = obj;
            lv_obj_set_pos(obj, 309, 60);
            lv_obj_set_size(obj, 112, 50);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff010101), LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000b2e), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xffdb01c0), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff00d9), LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // bmt_2
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.bmt_2 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffff00d9), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_thai_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_label_set_text(obj, "TUE");
                }
            }
        }
        {
            // bm_3
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.bm_3 = obj;
            lv_obj_set_pos(obj, 60, 137);
            lv_obj_set_size(obj, 112, 50);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000b2e), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff22cb00), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff22cb00), LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // bmt_3
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.bmt_3 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff22cb00), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_thai_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_label_set_text(obj, "WED");
                }
            }
        }
        {
            // bm_4
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.bm_4 = obj;
            lv_obj_set_pos(obj, 184, 137);
            lv_obj_set_size(obj, 112, 50);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000b2e), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xffff9b26), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff9b26), LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // bmt_4
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.bmt_4 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffff9b26), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_thai_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_label_set_text(obj, "THUR");
                }
            }
        }
        {
            // bm_5
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.bm_5 = obj;
            lv_obj_set_pos(obj, 310, 137);
            lv_obj_set_size(obj, 112, 50);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000b2e), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff00d1ff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff00d1ff), LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // bmt_5
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.bmt_5 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff00d1ff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_thai_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_label_set_text(obj, "FRI");
                }
            }
        }
        {
            // bm_6
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.bm_6 = obj;
            lv_obj_set_pos(obj, 184, 211);
            lv_obj_set_size(obj, 112, 50);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000b2e), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff8500ff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff8500ff), LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // bmt_6
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.bmt_6 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff8500ff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_thai_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_label_set_text(obj, "SAT");
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
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff121921), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // bm_label6
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.bm_label6 = obj;
            lv_obj_set_pos(obj, 259, 232);
            lv_obj_set_size(obj, 210, 50);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000b2e), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff2fabff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_PRESSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // d_label6
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.d_label6 = obj;
                    lv_obj_set_pos(obj, 36, 1);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff3d), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_label_set_text(obj, "Text");
                }
                {
                    // state_swich6
                    lv_obj_t *obj = lv_switch_create(parent_obj);
                    objects.state_swich6 = obj;
                    lv_obj_set_pos(obj, 132, 0);
                    lv_obj_set_size(obj, 50, 25);
                }
                {
                    // h_label6
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.h_label6 = obj;
                    lv_obj_set_pos(obj, -5, -3);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff3d), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_label_set_text(obj, "00");
                }
            }
        }
        {
            // bm_label4
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.bm_label4 = obj;
            lv_obj_set_pos(obj, 259, 167);
            lv_obj_set_size(obj, 210, 50);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000b2e), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff2fabff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_PRESSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // d_label4
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.d_label4 = obj;
                    lv_obj_set_pos(obj, 36, 1);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff3d), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_label_set_text(obj, "Text");
                }
                {
                    // h_label4
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.h_label4 = obj;
                    lv_obj_set_pos(obj, -5, -3);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff3d), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_label_set_text(obj, "00");
                }
                {
                    // state_swich4
                    lv_obj_t *obj = lv_switch_create(parent_obj);
                    objects.state_swich4 = obj;
                    lv_obj_set_pos(obj, 132, 0);
                    lv_obj_set_size(obj, 50, 25);
                }
            }
        }
        {
            // bm_label2
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.bm_label2 = obj;
            lv_obj_set_pos(obj, 259, 99);
            lv_obj_set_size(obj, 210, 50);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000b2e), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff2fabff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_PRESSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // d_label2
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.d_label2 = obj;
                    lv_obj_set_pos(obj, 36, 1);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff3d), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_label_set_text(obj, "Text");
                }
                {
                    // h_label2
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.h_label2 = obj;
                    lv_obj_set_pos(obj, -5, -3);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff3d), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_label_set_text(obj, "00");
                }
                {
                    // state_swich2
                    lv_obj_t *obj = lv_switch_create(parent_obj);
                    objects.state_swich2 = obj;
                    lv_obj_set_pos(obj, 132, 0);
                    lv_obj_set_size(obj, 50, 25);
                }
            }
        }
        {
            // bm_label5
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.bm_label5 = obj;
            lv_obj_set_pos(obj, 11, 232);
            lv_obj_set_size(obj, 210, 50);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000b2e), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff2fabff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_PRESSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // d_label5
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.d_label5 = obj;
                    lv_obj_set_pos(obj, 36, 1);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff3d), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_label_set_text(obj, "Text");
                }
                {
                    // h_label5
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.h_label5 = obj;
                    lv_obj_set_pos(obj, -3, -3);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff3d), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_label_set_text(obj, "00");
                }
                {
                    // state_swich5
                    lv_obj_t *obj = lv_switch_create(parent_obj);
                    objects.state_swich5 = obj;
                    lv_obj_set_pos(obj, 132, 0);
                    lv_obj_set_size(obj, 50, 25);
                }
            }
        }
        {
            // bm_label1
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.bm_label1 = obj;
            lv_obj_set_pos(obj, 11, 98);
            lv_obj_set_size(obj, 210, 50);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000b2e), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff2fabff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_PRESSED);
            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // d_label1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.d_label1 = obj;
                    lv_obj_set_pos(obj, 36, 1);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff3d), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_label_set_text(obj, "Text");
                }
                {
                    // h_label1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.h_label1 = obj;
                    lv_obj_set_pos(obj, -3, -3);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff3d), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_label_set_text(obj, "00");
                }
                {
                    // state_swich1
                    lv_obj_t *obj = lv_switch_create(parent_obj);
                    objects.state_swich1 = obj;
                    lv_obj_set_pos(obj, 132, 0);
                    lv_obj_set_size(obj, 50, 25);
                }
            }
        }
        {
            // bm_label3
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.bm_label3 = obj;
            lv_obj_set_pos(obj, 11, 166);
            lv_obj_set_size(obj, 210, 50);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000b2e), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff2fabff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_PRESSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // d_label3
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.d_label3 = obj;
                    lv_obj_set_pos(obj, 36, 1);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff3d), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_label_set_text(obj, "Text");
                }
                {
                    // h_label3
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.h_label3 = obj;
                    lv_obj_set_pos(obj, -3, -3);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff3d), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_label_set_text(obj, "00");
                }
                {
                    // state_swich3
                    lv_obj_t *obj = lv_switch_create(parent_obj);
                    objects.state_swich3 = obj;
                    lv_obj_set_pos(obj, 132, 0);
                    lv_obj_set_size(obj, 50, 25);
                }
            }
        }
        {
            // pg_switch_day
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.pg_switch_day = obj;
            lv_obj_set_pos(obj, 21, 58);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff2fabff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Text");
        }
    }
    
    tick_screen_pg_switch();
}

void tick_screen_pg_switch() {
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
            objects.obj14 = obj;
            lv_obj_set_pos(obj, -7, -19);
            lv_obj_set_size(obj, 117, 352);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff111744), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_clip_corner(obj, false, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_blend_mode(obj, LV_BLEND_MODE_NORMAL, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // num_save
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.num_save = obj;
                    lv_obj_set_pos(obj, 25, 158);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "0");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj15 = obj;
                    lv_obj_set_pos(obj, 13, 235);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &ui_font_thai_22, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "SAVE");
                }
                {
                    // pg_switch_day_1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.pg_switch_day_1 = obj;
                    lv_obj_set_pos(obj, 19, 66);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &ui_font_thai_22, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Text");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 112, 7);
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
                    lv_obj_set_style_text_font(obj, &ui_font_thai_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "START");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 112, 98);
            lv_obj_set_size(obj, 173, 85);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 113, -14);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &ui_font_thai_14, LV_PART_MAIN | LV_STATE_DEFAULT);
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
            lv_obj_set_pos(obj, 290, 99);
            lv_obj_set_size(obj, 167, 84);
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
                    lv_obj_set_style_text_font(obj, &ui_font_thai_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "END");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 290, 8);
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
                    lv_obj_set_style_text_font(obj, &ui_font_thai_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "START");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 112, 189);
            lv_obj_set_size(obj, 349, 78);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // spray_state
                    lv_obj_t *obj = lv_checkbox_create(parent_obj);
                    objects.spray_state = obj;
                    lv_obj_set_pos(obj, 69, 27);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_checkbox_set_text(obj, "SPRAY");
                    lv_obj_set_style_text_font(obj, &ui_font_thai_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // pump_state
                    lv_obj_t *obj = lv_checkbox_create(parent_obj);
                    objects.pump_state = obj;
                    lv_obj_set_pos(obj, 69, -2);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_checkbox_set_text(obj, "PUMP");
                    lv_obj_set_style_text_font(obj, &ui_font_thai_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // light_state
                    lv_obj_t *obj = lv_checkbox_create(parent_obj);
                    objects.light_state = obj;
                    lv_obj_set_pos(obj, -9, 27);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_checkbox_set_text(obj, "LIGHT");
                    lv_obj_set_style_text_font(obj, &ui_font_thai_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // bt_save
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.bt_save = obj;
                    lv_obj_set_pos(obj, 226, -3);
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
                    // fan_state
                    lv_obj_t *obj = lv_checkbox_create(parent_obj);
                    objects.fan_state = obj;
                    lv_obj_set_pos(obj, -9, -3);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_checkbox_set_text(obj, "FAN");
                    lv_obj_set_style_text_font(obj, &ui_font_thai_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // reserve_state
                    lv_obj_t *obj = lv_checkbox_create(parent_obj);
                    objects.reserve_state = obj;
                    lv_obj_set_pos(obj, 144, -2);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_checkbox_set_text(obj, "RESER");
                    lv_obj_set_style_text_font(obj, &ui_font_thai_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
    }
    
    tick_screen_set_time();
}

void tick_screen_set_time() {
}

void create_screen_pg_time() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.pg_time = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 320);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff121921), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj16 = obj;
            lv_obj_set_pos(obj, 9, 32);
            lv_obj_set_size(obj, 462, 257);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff2fabff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000b2e), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // set_min
                    lv_obj_t *obj = lv_dropdown_create(parent_obj);
                    objects.set_min = obj;
                    lv_obj_set_pos(obj, 137, 91);
                    lv_obj_set_size(obj, 113, LV_SIZE_CONTENT);
                    lv_dropdown_set_options(obj, "Min\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n51\n52\n53\n54\n55\n56\n57\n58\n59\n60");
                    lv_dropdown_set_selected(obj, 0);
                    lv_obj_set_style_border_width(obj, 3, LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_PRESSED);
                }
                {
                    // dd_mm_yy_4
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.dd_mm_yy_4 = obj;
                    lv_obj_set_pos(obj, 121, 92);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, ":");
                }
                {
                    // set_yy
                    lv_obj_t *obj = lv_dropdown_create(parent_obj);
                    objects.set_yy = obj;
                    lv_obj_set_pos(obj, 168, 33);
                    lv_obj_set_size(obj, 82, LV_SIZE_CONTENT);
                    lv_dropdown_set_options(obj, "YYYY\n2024\n2025\n2026\n2027\n2028\n2029\n2030\n2031\n2032\n2033\n2034\n2035\n2036\n2037\n2038\n2039\n2040\n2041\n2042\n2043\n2044\n2045\n2046\n2047\n2048\n2049\n2050\n2051\n2052\n2053\n2054\n2055\n2056\n2057\n2058\n2059\n2060\n2061\n2062\n2063\n2064\n2065\n2066\n2067\n2068\n2069\n2070\n2071\n2072\n2073\n2074\n2075\n2076\n2077\n2078\n2079\n2080\n2081\n2082\n2083\n2084\n2085\n2086\n2087\n2088\n2089\n2090\n2091\n2092\n2093\n2094\n2095\n2096\n2097\n2098\n2099\n2100");
                    lv_dropdown_set_selected(obj, 0);
                    lv_obj_set_style_border_width(obj, 3, LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_PRESSED);
                }
                {
                    // set_mm
                    lv_obj_t *obj = lv_dropdown_create(parent_obj);
                    objects.set_mm = obj;
                    lv_obj_set_pos(obj, 83, 33);
                    lv_obj_set_size(obj, 72, LV_SIZE_CONTENT);
                    lv_dropdown_set_options(obj, "MM\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12");
                    lv_dropdown_set_selected(obj, 0);
                    lv_obj_set_style_border_width(obj, 3, LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_PRESSED);
                }
                {
                    // set_dd
                    lv_obj_t *obj = lv_dropdown_create(parent_obj);
                    objects.set_dd = obj;
                    lv_obj_set_pos(obj, 2, 33);
                    lv_obj_set_size(obj, 65, LV_SIZE_CONTENT);
                    lv_dropdown_set_options(obj, "DD\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31");
                    lv_dropdown_set_selected(obj, 0);
                    lv_obj_set_style_border_width(obj, 3, LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_PRESSED);
                }
                {
                    // set_hh
                    lv_obj_t *obj = lv_dropdown_create(parent_obj);
                    objects.set_hh = obj;
                    lv_obj_set_pos(obj, 2, 91);
                    lv_obj_set_size(obj, 109, LV_SIZE_CONTENT);
                    lv_dropdown_set_options(obj, "HH\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n00");
                    lv_dropdown_set_selected(obj, 0);
                    lv_obj_set_style_border_width(obj, 3, LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_PRESSED);
                }
                {
                    // dd_mm_yy_6
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.dd_mm_yy_6 = obj;
                    lv_obj_set_pos(obj, 158, 35);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_26, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, ":");
                }
                {
                    // dd_mm_yy_5
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.dd_mm_yy_5 = obj;
                    lv_obj_set_pos(obj, 71, 35);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_26, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, ":");
                }
                {
                    // ddmmyy_1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ddmmyy_1 = obj;
                    lv_obj_set_pos(obj, 82, 178);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &ui_font_thai_30, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "00/00/00");
                }
                {
                    // timer_4
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.timer_4 = obj;
                    lv_obj_set_pos(obj, 254, 178);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &ui_font_thai_30, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "TIME:");
                }
                {
                    // timer_5
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.timer_5 = obj;
                    lv_obj_set_pos(obj, 1, 178);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &ui_font_thai_30, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "DAY :");
                }
                {
                    // timer_3
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.timer_3 = obj;
                    lv_obj_set_pos(obj, 339, 181);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &ui_font_thai_30, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "00:00");
                }
                {
                    // set_save
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.set_save = obj;
                    lv_obj_set_pos(obj, 297, 56);
                    lv_obj_set_size(obj, 125, 52);
                    lv_obj_set_style_radius(obj, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000b2e), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff58ff00), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_thai_22, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff63ff00), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 5, LV_PART_MAIN | LV_STATE_PRESSED);
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
            }
        }
    }
    
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
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff121921), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj17 = obj;
            lv_obj_set_pos(obj, 171, 13);
            lv_obj_set_size(obj, 140, 293);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_pad_left(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff2fabff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff001761), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // set_senser_1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.set_senser_1 = obj;
                    lv_obj_set_pos(obj, 26, 1);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff2fabff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "0");
                }
                {
                    // lbl_wind_val
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.lbl_wind_val = obj;
                    lv_obj_set_pos(obj, -27, 111);
                    lv_obj_set_size(obj, 46, 32);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_MOMENTUM);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff001761), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff0031fb), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff2a00ff), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_FULL, LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_border_width(obj, 4, LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff002394), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff2a00ff), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_border_width(obj, 4, LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_FULL, LV_PART_MAIN | LV_STATE_CHECKED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // set_senser
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.set_senser = obj;
                            lv_obj_set_pos(obj, 0, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                    }
                }
                {
                    // present_3
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.present_3 = obj;
                    lv_obj_set_pos(obj, -29, 157);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff2fabff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_label_set_text(obj, "M/S");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj18 = obj;
                    lv_obj_set_pos(obj, 0, -5);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &ui_font_thai_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "SENSOR");
                }
                {
                    // current_1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.current_1 = obj;
                    lv_obj_set_pos(obj, 0, 24);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffff0202), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "0");
                }
                {
                    // btn_set_high
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_set_high = obj;
                    lv_obj_set_pos(obj, 0, 1);
                    lv_obj_set_size(obj, 100, 42);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000b2e), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff35ff00), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff005809), LV_PART_MAIN | LV_STATE_PRESSED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj19 = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &ui_font_thai_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff35ff00), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "SETTING");
                        }
                    }
                }
                {
                    // present_2
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.present_2 = obj;
                    lv_obj_set_pos(obj, 0, 76);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &ui_font_thai_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff2fabff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff00a4ff), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_label_set_text(obj, "SET HIGH");
                }
                {
                    // bt1_1
                    lv_obj_t *obj = lv_spinbox_create(parent_obj);
                    objects.bt1_1 = obj;
                    lv_obj_set_pos(obj, 158, 34);
                    lv_obj_set_size(obj, 84, 46);
                    lv_spinbox_set_digit_format(obj, 3, 0);
                    lv_spinbox_set_range(obj, 0, 999);
                    lv_spinbox_set_rollover(obj, true);
                    lv_spinbox_set_step(obj, 1);
                    lv_spinbox_set_value(obj, 0);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // current_2
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.current_2 = obj;
                    lv_obj_set_pos(obj, 0, 50);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffff0202), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "0");
                }
                {
                    // present_6
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.present_6 = obj;
                    lv_obj_set_pos(obj, 26, 157);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff2fabff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_label_set_text(obj, "FT/m");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj20 = obj;
            lv_obj_set_pos(obj, 327, 13);
            lv_obj_set_size(obj, 146, 293);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_pad_left(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff2fabff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff001761), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // present_5
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.present_5 = obj;
                    lv_obj_set_pos(obj, 0, 92);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_add_state(obj, LV_STATE_CHECKED);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffdf6e00), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_label_set_text(obj, "--M/S--");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj21 = obj;
                    lv_obj_set_pos(obj, 0, -5);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &ui_font_thai_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffdf6e00), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "ALERT");
                }
                {
                    // btn_set_alert
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_set_alert = obj;
                    lv_obj_set_pos(obj, 0, 1);
                    lv_obj_set_size(obj, 100, 42);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000b2e), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xffff7f03), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff5f2f00), LV_PART_MAIN | LV_STATE_PRESSED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj22 = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &ui_font_thai_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xffff7e00), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "SET ALERT");
                        }
                    }
                }
                {
                    // lbl_alert_val
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.lbl_alert_val = obj;
                    lv_obj_set_pos(obj, 0, 36);
                    lv_obj_set_size(obj, 69, 32);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_MOMENTUM);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff001761), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff0031fb), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xffdf6e00), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_border_width(obj, 4, LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_FULL, LV_PART_MAIN | LV_STATE_PRESSED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // set_alert
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.set_alert = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj23 = obj;
            lv_obj_set_pos(obj, 11, 13);
            lv_obj_set_size(obj, 144, 293);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_pad_left(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff2fabff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff001761), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // present_4
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.present_4 = obj;
                    lv_obj_set_pos(obj, 0, 127);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_add_state(obj, LV_STATE_CHECKED);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff2fabff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_label_set_text(obj, "--M/S--");
                }
                {
                    // btn_set_zero
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_set_zero = obj;
                    lv_obj_set_pos(obj, 0, 1);
                    lv_obj_set_size(obj, 100, 42);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000b2e), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff35ff00), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff005809), LV_PART_MAIN | LV_STATE_PRESSED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj24 = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &ui_font_thai_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff35ff00), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "SET ZERO");
                        }
                    }
                }
                {
                    // current
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.current = obj;
                    lv_obj_set_pos(obj, 0, 24);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffff0202), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "0");
                }
                {
                    // present_1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.present_1 = obj;
                    lv_obj_set_pos(obj, 0, 91);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_add_state(obj, LV_STATE_CHECKED);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff2fabff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_label_set_text(obj, "0");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj25 = obj;
                    lv_obj_set_pos(obj, 0, -5);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &ui_font_thai_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "SENSOR");
                }
                {
                    // present
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.present = obj;
                    lv_obj_set_pos(obj, 0, 57);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_add_state(obj, LV_STATE_CHECKED);
                    lv_obj_set_style_text_font(obj, &ui_font_thai_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff2fabff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_label_set_text(obj, "SET LOW");
                }
            }
        }
    }
    
    tick_screen_pg_senser();
}

void tick_screen_pg_senser() {
}



typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main,
    tick_screen_pg_work,
    tick_screen_pg_switch,
    tick_screen_set_time,
    tick_screen_pg_time,
    tick_screen_pg_senser,
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
    create_screen_pg_work();
    create_screen_pg_switch();
    create_screen_set_time();
    create_screen_pg_time();
    create_screen_pg_senser();
}
