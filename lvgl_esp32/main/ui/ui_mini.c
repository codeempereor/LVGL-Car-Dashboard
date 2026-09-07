#include "ui_mini.h"
#include "st7789v.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static const char *TAG = "UI";

#define COLOR_BG     lv_color_hex(0x0d1117)
#define COLOR_CARD   lv_color_hex(0x161b22)
#define COLOR_CARD2  lv_color_hex(0x1c2128)
#define COLOR_TEXT   lv_color_hex(0xffffff)
#define COLOR_MUTED  lv_color_hex(0x8b949e)
#define COLOR_CYAN   lv_color_hex(0x00d4ff)
#define COLOR_AMBER  lv_color_hex(0xffaa00)
#define COLOR_GREEN  lv_color_hex(0x00ff88)
#define COLOR_RED    lv_color_hex(0xff4444)
#define COLOR_ARC_BG lv_color_hex(0x21262d)

static lv_obj_t *label_time;
static lv_obj_t *label_gear;
static lv_obj_t *label_speed;
static lv_obj_t *label_speed_unit;
static lv_obj_t *arc_speed;
static lv_obj_t *label_fuel_val;
static lv_obj_t *label_temp_val;
static lv_obj_t *label_range_val;
static lv_obj_t *label_odo;
static lv_obj_t *label_avg;
static lv_obj_t *label_bt;
static lv_obj_t *label_wifi;
static lv_obj_t *label_bat;

static int current_gear = 3;
static const char *gear_names[] = {"P", "R", "N", "D", "S"};
static lv_color_t gear_colors[5];

static float speed = 60.0f;
static int fuel = 75;
static int water_temp = 86;
static int range = 354;
static int odo = 12847;
static float avg_fuel = 6.5f;
static int tick_count = 0;

static lv_obj_t *create_card(lv_obj_t *parent, lv_coord_t x, lv_coord_t y,
                              lv_coord_t w, lv_coord_t h, const char *title)
{
    lv_obj_t *card = lv_obj_create(parent);
    lv_obj_set_size(card, w, h);
    lv_obj_set_pos(card, x, y);
    lv_obj_set_style_bg_color(card, COLOR_CARD, 0);
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(card, 0, 0);
    lv_obj_set_style_radius(card, 6, 0);
    lv_obj_set_style_pad_all(card, 4, 0);
    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *t = lv_label_create(card);
    lv_label_set_text(t, title);
    lv_obj_set_style_text_color(t, COLOR_MUTED, 0);
    lv_obj_set_style_text_font(t, &lv_font_montserrat_12, 0);
    lv_obj_align(t, LV_ALIGN_TOP_LEFT, 2, 0);

    return card;
}

void ui_mini_init(void)
{
    gear_colors[0] = lv_color_hex(0x00d4ff);
    gear_colors[1] = lv_color_hex(0xffaa00);
    gear_colors[2] = lv_color_hex(0x8b949e);
    gear_colors[3] = lv_color_hex(0x00ff88);
    gear_colors[4] = lv_color_hex(0xff4444);

    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, COLOR_BG, 0);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);

    lv_obj_t *top_bar = lv_obj_create(scr);
    lv_obj_set_size(top_bar, 320, 20);
    lv_obj_set_pos(top_bar, 0, 0);
    lv_obj_set_style_bg_color(top_bar, COLOR_CARD, 0);
    lv_obj_set_style_bg_opa(top_bar, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(top_bar, 0);
    lv_obj_set_style_radius(top_bar, 0);
    lv_obj_set_style_pad_all(top_bar, 0);
    lv_obj_clear_flag(top_bar, LV_OBJ_FLAG_SCROLLABLE);

    label_time = lv_label_create(top_bar);
    lv_label_set_text(label_time, "21:51");
    lv_obj_set_style_text_color(label_time, COLOR_TEXT, 0);
    lv_obj_set_style_text_font(label_time, &lv_font_montserrat_14, 0);
    lv_obj_align(label_time, LV_ALIGN_LEFT_MID, 8, 0);

    label_bat = lv_label_create(top_bar);
    lv_label_set_text(label_bat, LV_SYMBOL_BATTERY_FULL);
    lv_obj_set_style_text_color(label_bat, COLOR_GREEN, 0);
    lv_obj_set_style_text_font(label_bat, &lv_font_montserrat_14, 0);
    lv_obj_align(label_bat, LV_ALIGN_RIGHT_MID, -8, 0);

    label_wifi = lv_label_create(top_bar);
    lv_label_set_text(label_wifi, LV_SYMBOL_WIFI);
    lv_obj_set_style_text_color(label_wifi, COLOR_CYAN, 0);
    lv_obj_set_style_text_font(label_wifi, &lv_font_montserrat_14, 0);
    lv_obj_align(label_wifi, LV_ALIGN_RIGHT_MID, -28, 0);

    label_bt = lv_label_create(top_bar);
    lv_label_set_text(label_bt, LV_SYMBOL_BLUETOOTH);
    lv_obj_set_style_text_color(label_bt, COLOR_CYAN, 0);
    lv_obj_set_style_text_font(label_bt, &lv_font_montserrat_14, 0);
    lv_obj_align(label_bt, LV_ALIGN_RIGHT_MID, -48, 0);

    arc_speed = lv_arc_create(scr);
    lv_obj_set_size(arc_speed, 170, 170);
    lv_obj_set_pos(arc_speed, 5, 25);
    lv_arc_set_range(arc_speed, 0, 180);
    lv_arc_set_bg_angles(arc_speed, 135, 45);
    lv_arc_set_value(arc_speed, 60);
    lv_obj_remove_style(arc_speed, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(arc_speed, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_arc_color(arc_speed, COLOR_ARC_BG, LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc_speed, 10, LV_PART_MAIN);
    lv_obj_set_style_arc_color(arc_speed, COLOR_CYAN, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(arc_speed, 10, LV_PART_INDICATOR);

    lv_obj_t *tick0 = lv_label_create(scr);
    lv_label_set_text(tick0, "0");
    lv_obj_set_style_text_color(tick0, COLOR_MUTED, 0);
    lv_obj_set_style_text_font(tick0, &lv_font_montserrat_12, 0);
    lv_obj_set_pos(tick0, 12, 165);

    lv_obj_t *tick60 = lv_label_create(scr);
    lv_label_set_text(tick60, "60");
    lv_obj_set_style_text_color(tick60, COLOR_MUTED, 0);
    lv_obj_set_style_text_font(tick60, &lv_font_montserrat_12, 0);
    lv_obj_set_pos(tick60, 8, 60);

    lv_obj_t *tick120 = lv_label_create(scr);
    lv_label_set_text(tick120, "120");
    lv_obj_set_style_text_color(tick120, COLOR_MUTED, 0);
    lv_obj_set_style_text_font(tick120, &lv_font_montserrat_12, 0);
    lv_obj_set_pos(tick120, 130, 28);

    lv_obj_t *tick180 = lv_label_create(scr);
    lv_label_set_text(tick180, "180");
    lv_obj_set_style_text_color(tick180, COLOR_MUTED, 0);
    lv_obj_set_style_text_font(tick180, &lv_font_montserrat_12, 0);
    lv_obj_set_pos(tick180, 155, 100);

    label_speed = lv_label_create(scr);
    lv_label_set_text(label_speed, "60");
    lv_obj_set_style_text_color(label_speed, COLOR_TEXT, 0);
    lv_obj_set_style_text_font(label_speed, &lv_font_montserrat_28, 0);
    lv_obj_align(label_speed, LV_ALIGN_CENTER, -75, -5);

    label_speed_unit = lv_label_create(scr);
    lv_label_set_text(label_speed_unit, "km/h");
    lv_obj_set_style_text_color(label_speed_unit, COLOR_MUTED, 0);
    lv_obj_set_style_text_font(label_speed_unit, &lv_font_montserrat_14, 0);
    lv_obj_align(label_speed_unit, LV_ALIGN_CENTER, -75, 28);

    label_gear = lv_label_create(scr);
    lv_label_set_text(label_gear, "D");
    lv_obj_set_style_text_color(label_gear, COLOR_GREEN, 0);
    lv_obj_set_style_text_font(label_gear, &lv_font_montserrat_16, 0);
    lv_obj_align(label_gear, LV_ALIGN_CENTER, -75, 52);

    lv_obj_t *card_fuel = create_card(scr, 190, 28, 120, 55, "FUEL");
    label_fuel_val = lv_label_create(card_fuel);
    lv_label_set_text(label_fuel_val, "75%");
    lv_obj_set_style_text_color(label_fuel_val, COLOR_CYAN, 0);
    lv_obj_set_style_text_font(label_fuel_val, &lv_font_montserrat_16, 0);
    lv_obj_align(label_fuel_val, LV_ALIGN_BOTTOM_LEFT, 2, 0);

    lv_obj_t *card_temp = create_card(scr, 190, 88, 120, 55, "TEMP");
    label_temp_val = lv_label_create(card_temp);
    lv_label_set_text(label_temp_val, "86C");
    lv_obj_set_style_text_color(label_temp_val, COLOR_AMBER, 0);
    lv_obj_set_style_text_font(label_temp_val, &lv_font_montserrat_16, 0);
    lv_obj_align(label_temp_val, LV_ALIGN_BOTTOM_LEFT, 2, 0);

    lv_obj_t *card_range = create_card(scr, 190, 148, 120, 55, "RANGE");
    label_range_val = lv_label_create(card_range);
    lv_label_set_text(label_range_val, "354km");
    lv_obj_set_style_text_color(label_range_val, COLOR_GREEN, 0);
    lv_obj_set_style_text_font(label_range_val, &lv_font_montserrat_16, 0);
    lv_obj_align(label_range_val, LV_ALIGN_BOTTOM_LEFT, 2, 0);

    lv_obj_t *bottom_bar = lv_obj_create(scr);
    lv_obj_set_size(bottom_bar, 320, 20);
    lv_obj_set_pos(bottom_bar, 0, 220);
    lv_obj_set_style_bg_color(bottom_bar, COLOR_CARD, 0);
    lv_obj_set_style_bg_opa(bottom_bar, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(bottom_bar, 0);
    lv_obj_set_style_radius(bottom_bar, 0);
    lv_obj_set_style_pad_all(bottom_bar, 0);
    lv_obj_clear_flag(bottom_bar, LV_OBJ_FLAG_SCROLLABLE);

    label_odo = lv_label_create(bottom_bar);
    lv_label_set_text(label_odo, "12847 km");
    lv_obj_set_style_text_color(label_odo, COLOR_MUTED, 0);
    lv_obj_set_style_text_font(label_odo, &lv_font_montserrat_12, 0);
    lv_obj_align(label_odo, LV_ALIGN_LEFT_MID, 8, 0);

    label_avg = lv_label_create(bottom_bar);
    lv_label_set_text(label_avg, "6.5 L/100km");
    lv_obj_set_style_text_color(label_avg, COLOR_MUTED, 0);
    lv_obj_set_style_text_font(label_avg, &lv_font_montserrat_12, 0);
    lv_obj_align(label_avg, LV_ALIGN_RIGHT_MID, -8, 0);
}

void ui_mini_update(void)
{
    tick_count++;

    speed += 12.0f;
    if (speed > 180) speed = 0;

    ESP_LOGI(TAG, "update tick=%d speed=%.0f", tick_count, speed);

    char buf[16];
    snprintf(buf, sizeof(buf), "%d", (int)speed);
    lv_label_set_text(label_speed, buf);

    int32_t end_angle = 135 + (int32_t)((speed / 180.0f) * 270.0f);
    lv_arc_set_angles(arc_speed, 135, end_angle);
    lv_obj_invalidate(arc_speed);

    if (tick_count % 15 == 0) {
        current_gear = rand() % 5;
        lv_label_set_text(label_gear, gear_names[current_gear]);
        lv_obj_set_style_text_color(label_gear, gear_colors[current_gear], 0);
    }

    if (tick_count % 10 == 0) {
        fuel = 70 + rand() % 10;
        snprintf(buf, sizeof(buf), "%d%%", fuel);
        lv_label_set_text(label_fuel_val, buf);

        water_temp = 82 + rand() % 8;
        snprintf(buf, sizeof(buf), "%dC", water_temp);
        lv_label_set_text(label_temp_val, buf);

        range = 340 + rand() % 30;
        snprintf(buf, sizeof(buf), "%dkm", range);
        lv_label_set_text(label_range_val, buf);
    }

    if (tick_count % 50 == 0) {
        odo++;
        snprintf(buf, sizeof(buf), "%d km", odo);
        lv_label_set_text(label_odo, buf);

        avg_fuel = 6.0f + (float)(rand() % 10) / 10.0f;
        snprintf(buf, sizeof(buf), "%.1f L/100km", avg_fuel);
        lv_label_set_text(label_avg, buf);
    }
}
