#include "ui.h"
#include "app/app.h"
#include "app/lang.h"
#include <stdio.h>

static lv_obj_t *speed_meter;
static lv_obj_t *rpm_meter;
static lv_meter_scale_t *speed_scale;
static lv_meter_scale_t *rpm_scale;
static lv_meter_indicator_t *speed_indic;
static lv_meter_indicator_t *rpm_indic;
static lv_meter_indicator_t *rpm_redzone;
static lv_obj_t *speed_label;
static lv_obj_t *rpm_label;
static lv_obj_t *gear_items[5];
static lv_obj_t *fuel_val, *temp_val, *voltage_val, *odo_val, *range_val, *cons_val;
static lv_obj_t *fuel_title, *temp_title, *voltage_title, *odo_title, *range_title, *cons_title;
static lv_obj_t *center_title;
static lv_obj_t *center_cons_label, *center_range_label, *center_outtemp_label;
static lv_obj_t *center_cons, *center_range, *center_outtemp;
static lv_obj_t *left_arrow;
static lv_obj_t *right_arrow;

static void make_card(lv_obj_t *parent, lv_obj_t **val_out, lv_obj_t **title_out,
                       StrId title_id, const char *unit, int x, int y, int w)
{
    lv_obj_t *card = lv_obj_create(parent);
    lv_obj_set_size(card, w, 68);
    lv_obj_set_pos(card, x, y);
    lv_obj_set_style_bg_color(card, COLOR_CARD, 0);
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(card, COLOR_CARD2, 0);
    lv_obj_set_style_border_width(card, 1, 0);
    lv_obj_set_style_radius(card, 6, 0);
    lv_obj_set_style_pad_all(card, 8, 0);

    *title_out = lv_label_create(card);
    lv_label_set_text(*title_out, lang_get_str(title_id));
    lv_obj_set_style_text_color(*title_out, COLOR_MUTED, 0);
    lv_obj_set_style_text_font(*title_out, ui_font(12), 0); lv_obj_set_user_data(*title_out, (void*)(uintptr_t)12);
    lv_obj_align(*title_out, LV_ALIGN_TOP_LEFT, 0, 0);

    *val_out = lv_label_create(card);
    lv_label_set_text(*val_out, "--");
    lv_obj_set_style_text_color(*val_out, COLOR_TEXT, 0);
    lv_obj_set_style_text_font(*val_out, ui_font(16), 0); lv_obj_set_user_data(*val_out, (void*)(uintptr_t)16);
    lv_obj_align(*val_out, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    if (unit) {
        lv_obj_t *u = lv_label_create(card);
        lv_label_set_text(u, unit);
        lv_obj_set_style_text_color(u, COLOR_MUTED, 0);
        lv_obj_set_style_text_font(u, ui_font(12), 0); lv_obj_set_user_data(u, (void*)(uintptr_t)12);
        lv_obj_align(u, LV_ALIGN_BOTTOM_RIGHT, 0, 2);
    }
}

static void make_center_item(lv_obj_t *parent, lv_obj_t **val_out, lv_obj_t **label_out,
                              StrId label_id, int y)
{
    lv_obj_t *row = lv_obj_create(parent);
    lv_obj_set_size(row, lv_pct(100), 48);
    lv_obj_set_pos(row, 0, y);
    lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(row, 0, 0);
    lv_obj_set_style_pad_all(row, 0, 0);

    *label_out = lv_label_create(row);
    lv_label_set_text(*label_out, lang_get_str(label_id));
    lv_obj_set_style_text_color(*label_out, COLOR_MUTED, 0);
    lv_obj_set_style_text_font(*label_out, ui_font(12), 0); lv_obj_set_user_data(*label_out, (void*)(uintptr_t)12);
    lv_obj_align(*label_out, LV_ALIGN_LEFT_MID, 0, 0);

    *val_out = lv_label_create(row);
    lv_label_set_text(*val_out, "--");
    lv_obj_set_style_text_color(*val_out, COLOR_CYAN, 0);
    lv_obj_set_style_text_font(*val_out, ui_font(16), 0); lv_obj_set_user_data(*val_out, (void*)(uintptr_t)16);
    lv_obj_align(*val_out, LV_ALIGN_RIGHT_MID, 0, 0);
}

lv_obj_t* ui_create_dashboard(lv_obj_t* parent)
{
    left_arrow = lv_label_create(parent);
    lv_label_set_text(left_arrow, LV_SYMBOL_LEFT " " LV_SYMBOL_LEFT);
    lv_obj_set_style_text_color(left_arrow, COLOR_GREEN, 0);
    lv_obj_set_style_text_font(left_arrow, ui_font(20), 0); lv_obj_set_user_data(left_arrow, (void*)(uintptr_t)20);
    lv_obj_align(left_arrow, LV_ALIGN_TOP_LEFT, 30, 14);
    lv_obj_add_flag(left_arrow, LV_OBJ_FLAG_HIDDEN);

    right_arrow = lv_label_create(parent);
    lv_label_set_text(right_arrow, LV_SYMBOL_RIGHT " " LV_SYMBOL_RIGHT);
    lv_obj_set_style_text_color(right_arrow, COLOR_GREEN, 0);
    lv_obj_set_style_text_font(right_arrow, ui_font(20), 0); lv_obj_set_user_data(right_arrow, (void*)(uintptr_t)20);
    lv_obj_align(right_arrow, LV_ALIGN_TOP_RIGHT, -30, 14);
    lv_obj_add_flag(right_arrow, LV_OBJ_FLAG_HIDDEN);

    static const char *gear_labels[] = {"P", "R", "N", "D", "S"};
    for (int i = 0; i < 5; i++) {
        gear_items[i] = lv_label_create(parent);
        lv_label_set_text(gear_items[i], gear_labels[i]);
        lv_obj_set_style_text_color(gear_items[i], COLOR_MUTED, 0);
        lv_obj_set_style_text_font(gear_items[i], ui_font(20), 0); lv_obj_set_user_data(gear_items[i], (void*)(uintptr_t)20);
        lv_obj_align(gear_items[i], LV_ALIGN_TOP_MID, (i - 2) * 40, 10);
    }

    speed_meter = lv_meter_create(parent);
    lv_obj_set_size(speed_meter, 260, 260);
    lv_obj_align(speed_meter, LV_ALIGN_LEFT_MID, 40, -5);
    lv_obj_set_style_bg_color(speed_meter, COLOR_CARD, 0);
    lv_obj_set_style_bg_opa(speed_meter, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(speed_meter, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_color(speed_meter, COLOR_CYAN, 0);
    lv_obj_set_style_border_width(speed_meter, 2, 0);
    lv_obj_set_style_pad_all(speed_meter, 10, 0);

    speed_scale = lv_meter_add_scale(speed_meter);
    lv_meter_set_scale_ticks(speed_meter, speed_scale, 25, 1, 9, COLOR_MUTED);
    lv_meter_set_scale_major_ticks(speed_meter, speed_scale, 5, 2, 14, COLOR_SEC, 0);
    lv_meter_set_scale_range(speed_meter, speed_scale, 0, 240, 270, 135);
    speed_indic = lv_meter_add_needle_line(speed_meter, speed_scale, 3, COLOR_CYAN, -10);

    speed_label = lv_label_create(parent);
    lv_label_set_text(speed_label, "0");
    lv_obj_set_style_text_color(speed_label, COLOR_CYAN, 0);
    lv_obj_set_style_text_font(speed_label, ui_font(28), 0); lv_obj_set_user_data(speed_label, (void*)(uintptr_t)28);
    lv_obj_align_to(speed_label, speed_meter, LV_ALIGN_CENTER, 0, 12);

    lv_obj_t *speed_unit = lv_label_create(parent);
    lv_label_set_text(speed_unit, "km/h");
    lv_obj_set_style_text_color(speed_unit, COLOR_MUTED, 0);
    lv_obj_set_style_text_font(speed_unit, ui_font(12), 0); lv_obj_set_user_data(speed_unit, (void*)(uintptr_t)12);
    lv_obj_align_to(speed_unit, speed_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    rpm_meter = lv_meter_create(parent);
    lv_obj_set_size(rpm_meter, 260, 260);
    lv_obj_align(rpm_meter, LV_ALIGN_RIGHT_MID, -40, -5);
    lv_obj_set_style_bg_color(rpm_meter, COLOR_CARD, 0);
    lv_obj_set_style_bg_opa(rpm_meter, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(rpm_meter, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_color(rpm_meter, COLOR_BLUE, 0);
    lv_obj_set_style_border_width(rpm_meter, 2, 0);
    lv_obj_set_style_pad_all(rpm_meter, 10, 0);

    rpm_scale = lv_meter_add_scale(rpm_meter);
    lv_meter_set_scale_ticks(rpm_meter, rpm_scale, 17, 1, 9, COLOR_MUTED);
    lv_meter_set_scale_major_ticks(rpm_meter, rpm_scale, 2, 2, 14, COLOR_SEC, 0);
    lv_meter_set_scale_range(rpm_meter, rpm_scale, 0, 8000, 270, 135);
    rpm_redzone = lv_meter_add_arc(rpm_meter, rpm_scale, 4, COLOR_RED, 0);
    lv_meter_set_indicator_start_value(rpm_meter, rpm_redzone, 6000);
    rpm_indic = lv_meter_add_needle_line(rpm_meter, rpm_scale, 3, COLOR_BLUE, -10);

    rpm_label = lv_label_create(parent);
    lv_label_set_text(rpm_label, "0.8");
    lv_obj_set_style_text_color(rpm_label, COLOR_BLUE, 0);
    lv_obj_set_style_text_font(rpm_label, ui_font(28), 0); lv_obj_set_user_data(rpm_label, (void*)(uintptr_t)28);
    lv_obj_align_to(rpm_label, rpm_meter, LV_ALIGN_CENTER, 0, 12);

    lv_obj_t *rpm_unit = lv_label_create(parent);
    lv_label_set_text(rpm_unit, "x1000 rpm");
    lv_obj_set_style_text_color(rpm_unit, COLOR_MUTED, 0);
    lv_obj_set_style_text_font(rpm_unit, ui_font(12), 0); lv_obj_set_user_data(rpm_unit, (void*)(uintptr_t)12);
    lv_obj_align_to(rpm_unit, rpm_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    lv_obj_t *center_panel = lv_obj_create(parent);
    lv_obj_set_size(center_panel, 300, 180);
    lv_obj_align(center_panel, LV_ALIGN_CENTER, 0, -10);
    lv_obj_set_style_bg_color(center_panel, COLOR_CARD, 0);
    lv_obj_set_style_bg_opa(center_panel, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(center_panel, COLOR_CARD2, 0);
    lv_obj_set_style_border_width(center_panel, 1, 0);
    lv_obj_set_style_radius(center_panel, 8, 0);
    lv_obj_set_style_pad_all(center_panel, 14, 0);

    center_title = lv_label_create(center_panel);
    lv_label_set_text(center_title, lang_get_str(STR_VEHICLE_STATUS));
    lv_obj_set_style_text_color(center_title, COLOR_SEC, 0);
    lv_obj_set_style_text_font(center_title, ui_font(12), 0); lv_obj_set_user_data(center_title, (void*)(uintptr_t)12);
    lv_obj_align(center_title, LV_ALIGN_TOP_MID, 0, 0);

    make_center_item(center_panel, &center_cons, &center_cons_label, STR_INSTANT_CONS, 28);
    make_center_item(center_panel, &center_range, &center_range_label, STR_RANGE, 76);
    make_center_item(center_panel, &center_outtemp, &center_outtemp_label, STR_OUTDOOR_TEMP, 124);

    static const char *status_names[] = {"ABS", "CHECK", "BAT", "TIRE", "OIL", "AIRBAG"};
    int icon_total = 6 * 70;
    int icon_start = (1024 - icon_total) / 2;
    for (int i = 0; i < 6; i++) {
        lv_obj_t *grp = lv_obj_create(parent);
        lv_obj_set_size(grp, 64, 36);
        lv_obj_set_pos(grp, icon_start + i * 70, 368);
        lv_obj_set_style_bg_opa(grp, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(grp, 0, 0);
        lv_obj_set_style_pad_all(grp, 0, 0);

        lv_obj_t *led = lv_led_create(grp);
        lv_obj_set_size(led, 10, 10);
        lv_obj_align(led, LV_ALIGN_LEFT_MID, 0, 0);
        lv_led_set_color(led, COLOR_AMBER);
        lv_led_off(led);

        lv_obj_t *nm = lv_label_create(grp);
        lv_label_set_text(nm, status_names[i]);
        lv_obj_set_style_text_color(nm, COLOR_MUTED, 0);
        lv_obj_set_style_text_font(nm, ui_font(12), 0); lv_obj_set_user_data(nm, (void*)(uintptr_t)12);
        lv_obj_align(nm, LV_ALIGN_RIGHT_MID, 0, 0);
    }

    int card_w = 148;
    int card_gap = 12;
    int total_w = 6 * card_w + 5 * card_gap;
    int start_x = (1024 - total_w) / 2;
    int card_y = 432;

    make_card(parent, &fuel_val, &fuel_title, STR_FUEL, "%", start_x, card_y, card_w);
    make_card(parent, &temp_val, &temp_title, STR_COOLANT, "C", start_x + (card_w + card_gap), card_y, card_w);
    make_card(parent, &voltage_val, &voltage_title, STR_BATTERY, "V", start_x + 2*(card_w + card_gap), card_y, card_w);
    make_card(parent, &odo_val, &odo_title, STR_ODOMETER, "km", start_x + 3*(card_w + card_gap), card_y, card_w);
    make_card(parent, &range_val, &range_title, STR_RANGE, "km", start_x + 4*(card_w + card_gap), card_y, card_w);
    make_card(parent, &cons_val, &cons_title, STR_AVG_CONS, "L/100", start_x + 5*(card_w + card_gap), card_y, card_w);

    return parent;
}

void ui_dashboard_refresh_lang(void)
{
    lv_label_set_text(center_title, lang_get_str(STR_VEHICLE_STATUS));
    lv_label_set_text(center_cons_label, lang_get_str(STR_INSTANT_CONS));
    lv_label_set_text(center_range_label, lang_get_str(STR_RANGE));
    lv_label_set_text(center_outtemp_label, lang_get_str(STR_OUTDOOR_TEMP));
    lv_label_set_text(fuel_title, lang_get_str(STR_FUEL));
    lv_label_set_text(temp_title, lang_get_str(STR_COOLANT));
    lv_label_set_text(voltage_title, lang_get_str(STR_BATTERY));
    lv_label_set_text(odo_title, lang_get_str(STR_ODOMETER));
    lv_label_set_text(range_title, lang_get_str(STR_RANGE));
    lv_label_set_text(cons_title, lang_get_str(STR_AVG_CONS));
}

void ui_dashboard_update(void)
{
    VehicleState *vs = app_get_state();
    char buf[32];

    snprintf(buf, sizeof(buf), "%u", vs->speed);
    lv_label_set_text(speed_label, buf);
    lv_meter_set_indicator_value(speed_meter, speed_indic, vs->speed);

    snprintf(buf, sizeof(buf), "%.1f", vs->rpm / 1000.0);
    lv_label_set_text(rpm_label, buf);
    lv_meter_set_indicator_value(rpm_meter, rpm_indic, vs->rpm);

    for (int i = 0; i < 5; i++) {
        if (i == vs->gear) {
            lv_obj_set_style_text_color(gear_items[i], COLOR_AMBER, 0);
            lv_obj_set_style_text_font(gear_items[i], ui_font(28), 0); lv_obj_set_user_data(gear_items[i], (void*)(uintptr_t)28);
        } else {
            lv_obj_set_style_text_color(gear_items[i], COLOR_MUTED, 0);
            lv_obj_set_style_text_font(gear_items[i], ui_font(20), 0); lv_obj_set_user_data(gear_items[i], (void*)(uintptr_t)20);
        }
    }

    snprintf(buf, sizeof(buf), "%u", vs->fuel);
    lv_label_set_text(fuel_val, buf);
    snprintf(buf, sizeof(buf), "%u", vs->coolant_temp);
    lv_label_set_text(temp_val, buf);
    snprintf(buf, sizeof(buf), "%.1f", 12.4 + (vs->speed % 10) * 0.05);
    lv_label_set_text(voltage_val, buf);

    static uint32_t odo = 12847;
    odo += vs->speed / 360;
    snprintf(buf, sizeof(buf), "%u", odo);
    lv_label_set_text(odo_val, buf);

    uint32_t range = (vs->fuel * 6);
    snprintf(buf, sizeof(buf), "%u", range);
    lv_label_set_text(range_val, buf);

    float cons = 5.0f + (vs->speed / 40.0f);
    snprintf(buf, sizeof(buf), "%.1f", cons);
    lv_label_set_text(cons_val, buf);
    lv_label_set_text(center_cons, buf);
    snprintf(buf, sizeof(buf), "%u km", range);
    lv_label_set_text(center_range, buf);
    lv_label_set_text(center_outtemp, "24 C");

    if (vs->left_turn) lv_obj_clear_flag(left_arrow, LV_OBJ_FLAG_HIDDEN);
    else lv_obj_add_flag(left_arrow, LV_OBJ_FLAG_HIDDEN);
    if (vs->right_turn) lv_obj_clear_flag(right_arrow, LV_OBJ_FLAG_HIDDEN);
    else lv_obj_add_flag(right_arrow, LV_OBJ_FLAG_HIDDEN);
}
