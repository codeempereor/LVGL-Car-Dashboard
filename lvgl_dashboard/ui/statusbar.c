#include "ui.h"
#include "app/app.h"
#include "app/lang.h"
#include <time.h>
#include <stdio.h>

static lv_obj_t *time_label;
static lv_obj_t *date_label;
static lv_obj_t *temp_label;
static lv_obj_t *left_turn_led;
static lv_obj_t *right_turn_led;
static lv_obj_t *headlight_led;

static const char *months_en[] = {"JAN","FEB","MAR","APR","MAY","JUN","JUL","AUG","SEP","OCT","NOV","DEC"};
static const char *months_cn[] = {"1月","2月","3月","4月","5月","6月","7月","8月","9月","10月","11月","12月"};

static void status_timer_cb(lv_timer_t *timer)
{
    (void)timer;
    VehicleState *vs = app_get_state();

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char buf[32];
    snprintf(buf, sizeof(buf), "%02d:%02d", t->tm_hour, t->tm_min);
    lv_label_set_text(time_label, buf);

    const char **months = (lang_get() == LANG_CN) ? months_cn : months_en;
    snprintf(buf, sizeof(buf), "%s %02d", months[t->tm_mon], t->tm_mday);
    lv_label_set_text(date_label, buf);

    if (vs->left_turn) { lv_led_set_color(left_turn_led, COLOR_GREEN); lv_led_on(left_turn_led); }
    else lv_led_off(left_turn_led);
    if (vs->right_turn) { lv_led_set_color(right_turn_led, COLOR_GREEN); lv_led_on(right_turn_led); }
    else lv_led_off(right_turn_led);
    if (vs->headlight) { lv_led_set_color(headlight_led, COLOR_CYAN); lv_led_on(headlight_led); }
    else lv_led_off(headlight_led);
}

void ui_statusbar_refresh_lang(void)
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char buf[32];
    const char **months = (lang_get() == LANG_CN) ? months_cn : months_en;
    snprintf(buf, sizeof(buf), "%s %02d", months[t->tm_mon], t->tm_mday);
    lv_label_set_text(date_label, buf);
}

lv_obj_t* ui_create_statusbar(lv_obj_t* parent)
{
    lv_obj_t *bar = lv_obj_create(parent);
    lv_obj_set_width(bar, lv_pct(100));
    lv_obj_set_height(bar, 44);
    lv_obj_set_style_bg_color(bar, COLOR_CARD, 0);
    lv_obj_set_style_bg_opa(bar, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(bar, 0, 0);
    lv_obj_set_style_border_color(bar, COLOR_CARD2, 0);
    lv_obj_set_style_border_side(bar, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_width(bar, 1, 0);
    lv_obj_set_style_radius(bar, 0, 0);
    lv_obj_set_style_pad_all(bar, 0, 0);

    lv_obj_t *left_group = lv_obj_create(bar);
    lv_obj_set_size(left_group, 220, LV_SIZE_CONTENT);
    lv_obj_align(left_group, LV_ALIGN_LEFT_MID, 12, 0);
    lv_obj_set_style_bg_opa(left_group, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(left_group, 0, 0);
    lv_obj_set_style_pad_all(left_group, 0, 0);
    lv_obj_set_flex_flow(left_group, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(left_group, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(left_group, 14, 0);

    temp_label = lv_label_create(left_group);
    lv_label_set_text(temp_label, "24 C");
    lv_obj_set_style_text_color(temp_label, COLOR_AMBER, 0);
    lv_obj_set_style_text_font(temp_label, ui_font(14), 0); lv_obj_set_user_data(temp_label, (void*)(uintptr_t)14);

    lv_obj_t *bt_icon = lv_label_create(left_group);
    lv_label_set_text(bt_icon, LV_SYMBOL_BLUETOOTH);
    lv_obj_set_style_text_color(bt_icon, COLOR_BLUE, 0);
    lv_obj_set_style_text_font(bt_icon, ui_font(14), 0); lv_obj_set_user_data(bt_icon, (void*)(uintptr_t)14);

    lv_obj_t *wifi_icon = lv_label_create(left_group);
    lv_label_set_text(wifi_icon, LV_SYMBOL_WIFI);
    lv_obj_set_style_text_color(wifi_icon, COLOR_GREEN, 0);
    lv_obj_set_style_text_font(wifi_icon, ui_font(14), 0); lv_obj_set_user_data(wifi_icon, (void*)(uintptr_t)14);

    lv_obj_t *center_group = lv_obj_create(bar);
    lv_obj_set_size(center_group, 140, LV_SIZE_CONTENT);
    lv_obj_align(center_group, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_opa(center_group, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(center_group, 0, 0);
    lv_obj_set_style_pad_all(center_group, 0, 0);
    lv_obj_set_flex_flow(center_group, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(center_group, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    time_label = lv_label_create(center_group);
    lv_label_set_text(time_label, "--:--");
    lv_obj_set_style_text_color(time_label, COLOR_TEXT, 0);
    lv_obj_set_style_text_font(time_label, ui_font(20), 0); lv_obj_set_user_data(time_label, (void*)(uintptr_t)20);

    date_label = lv_label_create(center_group);
    lv_label_set_text(date_label, "-- ---");
    lv_obj_set_style_text_color(date_label, COLOR_MUTED, 0);
    lv_obj_set_style_text_font(date_label, ui_font(12), 0); lv_obj_set_user_data(date_label, (void*)(uintptr_t)12);

    lv_obj_t *right_group = lv_obj_create(bar);
    lv_obj_set_size(right_group, 200, LV_SIZE_CONTENT);
    lv_obj_align(right_group, LV_ALIGN_RIGHT_MID, -12, 0);
    lv_obj_set_style_bg_opa(right_group, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(right_group, 0, 0);
    lv_obj_set_style_pad_all(right_group, 0, 0);
    lv_obj_set_flex_flow(right_group, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(right_group, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(right_group, 12, 0);

    left_turn_led = lv_led_create(right_group);
    lv_obj_set_size(left_turn_led, 12, 12);
    lv_led_off(left_turn_led);

    lv_obj_t *parking_icon = lv_label_create(right_group);
    lv_label_set_text(parking_icon, "P");
    lv_obj_set_style_text_color(parking_icon, COLOR_RED, 0);
    lv_obj_set_style_text_font(parking_icon, ui_font(16), 0); lv_obj_set_user_data(parking_icon, (void*)(uintptr_t)16);

    lv_obj_t *seatbelt_icon = lv_label_create(right_group);
    lv_label_set_text(seatbelt_icon, LV_SYMBOL_WARNING);
    lv_obj_set_style_text_color(seatbelt_icon, COLOR_AMBER, 0);
    lv_obj_set_style_text_font(seatbelt_icon, ui_font(14), 0); lv_obj_set_user_data(seatbelt_icon, (void*)(uintptr_t)14);

    headlight_led = lv_led_create(right_group);
    lv_obj_set_size(headlight_led, 12, 12);
    lv_led_off(headlight_led);

    right_turn_led = lv_led_create(right_group);
    lv_obj_set_size(right_turn_led, 12, 12);
    lv_led_off(right_turn_led);

    lv_timer_create(status_timer_cb, 500, NULL);

    return bar;
}
