#include "ui.h"
#include "app/app.h"
#include "app/lang.h"
#include <stdio.h>

static lv_obj_t *brightness_slider;
static lv_obj_t *brightness_val;
static lv_obj_t *ac_temp_label;
static lv_obj_t *ac_btn_label;
static lv_obj_t *gear_title, *light_title, *bright_title, *climate_title;
static lv_obj_t *left_btn_label, *head_btn_label, *right_btn_label;
static lv_obj_t *lang_btn_label;
static int ac_temp = 22;
static bool ac_on = true;

static void gear_btn_event_cb(lv_event_t *e)
{
    uintptr_t g = (uintptr_t)lv_event_get_user_data(e);
    app_set_gear((GearType)g);
}

static void brightness_event_cb(lv_event_t *e)
{
    lv_obj_t *slider = lv_event_get_target(e);
    uint8_t val = (uint8_t)lv_slider_get_value(slider);
    app_set_brightness(val);
    char buf[16];
    snprintf(buf, sizeof(buf), "%u%%", val);
    lv_label_set_text(brightness_val, buf);
}

static void left_turn_event_cb(lv_event_t *e) { (void)e; app_toggle_left_turn(); }
static void right_turn_event_cb(lv_event_t *e) { (void)e; app_toggle_right_turn(); }
static void headlight_event_cb(lv_event_t *e) { (void)e; app_toggle_headlight(); }

static void ac_temp_up_cb(lv_event_t *e)
{
    (void)e;
    if (ac_temp < 30) ac_temp++;
    char buf[16];
    snprintf(buf, sizeof(buf), "%d C", ac_temp);
    lv_label_set_text(ac_temp_label, buf);
}

static void ac_temp_down_cb(lv_event_t *e)
{
    (void)e;
    if (ac_temp > 16) ac_temp--;
    char buf[16];
    snprintf(buf, sizeof(buf), "%d C", ac_temp);
    lv_label_set_text(ac_temp_label, buf);
}

static void ac_toggle_cb(lv_event_t *e)
{
    (void)e;
    ac_on = !ac_on;
    lv_label_set_text(ac_btn_label, ac_on ? lang_get_str(STR_AC_ON) : lang_get_str(STR_AC_OFF));
}

static void lang_toggle_cb(lv_event_t *e)
{
    (void)e;
    lang_toggle();
    ui_refresh_language();
}

static lv_obj_t* make_panel(lv_obj_t *parent, lv_obj_t **title_out, StrId title_id, int x, int y, int w, int h)
{
    lv_obj_t *panel = lv_obj_create(parent);
    lv_obj_set_size(panel, w, h);
    lv_obj_set_pos(panel, x, y);
    lv_obj_set_style_bg_color(panel, COLOR_CARD, 0);
    lv_obj_set_style_bg_opa(panel, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(panel, COLOR_CARD2, 0);
    lv_obj_set_style_border_width(panel, 1, 0);
    lv_obj_set_style_radius(panel, 10, 0);
    lv_obj_set_style_pad_all(panel, 14, 0);

    *title_out = lv_label_create(panel);
    lv_label_set_text(*title_out, lang_get_str(title_id));
    lv_obj_set_style_text_color(*title_out, COLOR_CYAN, 0);
    lv_obj_set_style_text_font(*title_out, ui_font(16), 0); lv_obj_set_user_data(*title_out, (void*)(uintptr_t)16);
    lv_obj_align(*title_out, LV_ALIGN_TOP_LEFT, 4, 4);

    return panel;
}

void ui_control_refresh_lang(void)
{
    lv_label_set_text(gear_title, lang_get_str(STR_TRANSMISSION));
    lv_label_set_text(light_title, lang_get_str(STR_LIGHTING));
    lv_label_set_text(bright_title, lang_get_str(STR_DISPLAY_BRIGHTNESS));
    lv_label_set_text(climate_title, lang_get_str(STR_CLIMATE_CONTROL));
    lv_label_set_text(left_btn_label, lang_get_str(STR_LEFT_TURN));
    lv_label_set_text(head_btn_label, lang_get_str(STR_HEADLIGHT));
    lv_label_set_text(right_btn_label, lang_get_str(STR_RIGHT_TURN));
    lv_label_set_text(ac_btn_label, ac_on ? lang_get_str(STR_AC_ON) : lang_get_str(STR_AC_OFF));
    lv_label_set_text(lang_btn_label, (lang_get() == LANG_CN) ? "EN" : "中文");
}

lv_obj_t* ui_create_control_page(lv_obj_t* parent)
{
    int pw = 470, ph = 210, gap = 16;
    int sx = (1024 - 2 * pw - gap) / 2;
    int sy = 8;

    lv_obj_t *gear_panel = make_panel(parent, &gear_title, STR_TRANSMISSION, sx, sy, pw, ph);
    static const char *gears[] = {"P", "R", "N", "D", "S"};
    for (int i = 0; i < 5; i++) {
        lv_obj_t *btn = lv_btn_create(gear_panel);
        lv_obj_set_size(btn, 64, 70);
        lv_obj_align(btn, LV_ALIGN_CENTER, (i - 2) * 78, 10);
        lv_obj_set_style_bg_color(btn, COLOR_CARD2, 0);
        lv_obj_set_style_bg_color(btn, COLOR_CYAN, LV_STATE_PRESSED);
        lv_obj_set_style_radius(btn, 8, 0);
        lv_obj_t *l = lv_label_create(btn);
        lv_label_set_text(l, gears[i]);
        lv_obj_set_style_text_color(l, COLOR_TEXT, 0);
        lv_obj_set_style_text_font(l, ui_font(24), 0); lv_obj_set_user_data(l, (void*)(uintptr_t)24);
        lv_obj_center(l);
        lv_obj_add_event_cb(btn, gear_btn_event_cb, LV_EVENT_CLICKED, (void *)(uintptr_t)i);
    }

    lv_obj_t *light_panel = make_panel(parent, &light_title, STR_LIGHTING, sx + pw + gap, sy, pw, ph);
    StrId light_ids[] = {STR_LEFT_TURN, STR_HEADLIGHT, STR_RIGHT_TURN};
    lv_obj_t **light_labels[] = {&left_btn_label, &head_btn_label, &right_btn_label};
    lv_event_cb_t light_cbs[] = {left_turn_event_cb, headlight_event_cb, right_turn_event_cb};
    for (int i = 0; i < 3; i++) {
        lv_obj_t *btn = lv_btn_create(light_panel);
        lv_obj_set_size(btn, 120, 60);
        lv_obj_align(btn, LV_ALIGN_CENTER, (i - 1) * 140, 10);
        lv_obj_set_style_bg_color(btn, COLOR_CARD2, 0);
        lv_obj_set_style_bg_color(btn, COLOR_AMBER, LV_STATE_PRESSED);
        lv_obj_set_style_radius(btn, 8, 0);
        *light_labels[i] = lv_label_create(btn);
        lv_label_set_text(*light_labels[i], lang_get_str(light_ids[i]));
        lv_obj_set_style_text_color(*light_labels[i], COLOR_TEXT, 0);
        lv_obj_set_style_text_font(*light_labels[i], ui_font(14), 0); lv_obj_set_user_data(*light_labels[i], (void*)(uintptr_t)14);
        lv_obj_center(*light_labels[i]);
        lv_obj_add_event_cb(btn, light_cbs[i], LV_EVENT_CLICKED, NULL);
    }

    int sy2 = sy + ph + gap;
    lv_obj_t *bright_panel = make_panel(parent, &bright_title, STR_DISPLAY_BRIGHTNESS, sx, sy2, pw, ph);
    brightness_slider = lv_slider_create(bright_panel);
    lv_slider_set_range(brightness_slider, 0, 100);
    lv_slider_set_value(brightness_slider, 80, LV_ANIM_OFF);
    lv_obj_set_size(brightness_slider, 340, 14);
    lv_obj_align(brightness_slider, LV_ALIGN_CENTER, -20, 10);
    lv_obj_set_style_bg_color(brightness_slider, COLOR_CARD2, LV_PART_MAIN);
    lv_obj_set_style_bg_color(brightness_slider, COLOR_CYAN, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(brightness_slider, COLOR_TEXT, LV_PART_KNOB);
    lv_obj_add_event_cb(brightness_slider, brightness_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    brightness_val = lv_label_create(bright_panel);
    lv_label_set_text(brightness_val, "80%");
    lv_obj_set_style_text_color(brightness_val, COLOR_CYAN, 0);
    lv_obj_set_style_text_font(brightness_val, ui_font(20), 0); lv_obj_set_user_data(brightness_val, (void*)(uintptr_t)20);
    lv_obj_align(brightness_val, LV_ALIGN_RIGHT_MID, -10, 10);

    lv_obj_t *ac_panel = make_panel(parent, &climate_title, STR_CLIMATE_CONTROL, sx + pw + gap, sy2, pw, ph);

    lv_obj_t *ac_down = lv_btn_create(ac_panel);
    lv_obj_set_size(ac_down, 56, 56);
    lv_obj_align(ac_down, LV_ALIGN_CENTER, -100, 10);
    lv_obj_set_style_bg_color(ac_down, COLOR_CARD2, 0);
    lv_obj_set_style_bg_color(ac_down, COLOR_BLUE, LV_STATE_PRESSED);
    lv_obj_set_style_radius(ac_down, LV_RADIUS_CIRCLE, 0);
    lv_obj_t *dl = lv_label_create(ac_down);
    lv_label_set_text(dl, LV_SYMBOL_MINUS);
    lv_obj_set_style_text_color(dl, COLOR_TEXT, 0);
    lv_obj_set_style_text_font(dl, ui_font(24), 0); lv_obj_set_user_data(dl, (void*)(uintptr_t)24);
    lv_obj_center(dl);
    lv_obj_add_event_cb(ac_down, ac_temp_down_cb, LV_EVENT_CLICKED, NULL);

    ac_temp_label = lv_label_create(ac_panel);
    lv_label_set_text(ac_temp_label, "22 C");
    lv_obj_set_style_text_color(ac_temp_label, COLOR_CYAN, 0);
    lv_obj_set_style_text_font(ac_temp_label, ui_font(28), 0); lv_obj_set_user_data(ac_temp_label, (void*)(uintptr_t)28);
    lv_obj_align(ac_temp_label, LV_ALIGN_CENTER, 0, 10);

    lv_obj_t *ac_up = lv_btn_create(ac_panel);
    lv_obj_set_size(ac_up, 56, 56);
    lv_obj_align(ac_up, LV_ALIGN_CENTER, 100, 10);
    lv_obj_set_style_bg_color(ac_up, COLOR_CARD2, 0);
    lv_obj_set_style_bg_color(ac_up, COLOR_RED, LV_STATE_PRESSED);
    lv_obj_set_style_radius(ac_up, LV_RADIUS_CIRCLE, 0);
    lv_obj_t *ul = lv_label_create(ac_up);
    lv_label_set_text(ul, LV_SYMBOL_PLUS);
    lv_obj_set_style_text_color(ul, COLOR_TEXT, 0);
    lv_obj_set_style_text_font(ul, ui_font(24), 0); lv_obj_set_user_data(ul, (void*)(uintptr_t)24);
    lv_obj_center(ul);
    lv_obj_add_event_cb(ac_up, ac_temp_up_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *ac_toggle = lv_btn_create(ac_panel);
    lv_obj_set_size(ac_toggle, 80, 40);
    lv_obj_align(ac_toggle, LV_ALIGN_RIGHT_MID, -10, 10);
    lv_obj_set_style_bg_color(ac_toggle, COLOR_CARD2, 0);
    lv_obj_set_style_bg_color(ac_toggle, COLOR_GREEN, LV_STATE_PRESSED);
    lv_obj_set_style_radius(ac_toggle, 8, 0);
    ac_btn_label = lv_label_create(ac_toggle);
    lv_label_set_text(ac_btn_label, lang_get_str(STR_AC_ON));
    lv_obj_set_style_text_color(ac_btn_label, COLOR_TEXT, 0);
    lv_obj_set_style_text_font(ac_btn_label, ui_font(14), 0); lv_obj_set_user_data(ac_btn_label, (void*)(uintptr_t)14);
    lv_obj_center(ac_btn_label);
    lv_obj_add_event_cb(ac_toggle, ac_toggle_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *lang_btn = lv_btn_create(parent);
    lv_obj_set_size(lang_btn, 120, 40);
    lv_obj_align(lang_btn, LV_ALIGN_BOTTOM_MID, 0, -12);
    lv_obj_set_style_bg_color(lang_btn, COLOR_CARD2, 0);
    lv_obj_set_style_bg_color(lang_btn, COLOR_CYAN, LV_STATE_PRESSED);
    lv_obj_set_style_radius(lang_btn, 8, 0);
    lang_btn_label = lv_label_create(lang_btn);
    lv_label_set_text(lang_btn_label, (lang_get() == LANG_CN) ? "EN" : "中文");
    lv_obj_set_style_text_color(lang_btn_label, COLOR_TEXT, 0);
    lv_obj_set_style_text_font(lang_btn_label, ui_font(16), 0); lv_obj_set_user_data(lang_btn_label, (void*)(uintptr_t)16);
    lv_obj_center(lang_btn_label);
    lv_obj_add_event_cb(lang_btn, lang_toggle_cb, LV_EVENT_CLICKED, NULL);

    return parent;
}
