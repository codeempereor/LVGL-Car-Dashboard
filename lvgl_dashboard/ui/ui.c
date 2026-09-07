#include "ui.h"
#include "app/app.h"
#include "app/lang.h"

LV_FONT_DECLARE(font_cn_12);
LV_FONT_DECLARE(font_cn_14);
LV_FONT_DECLARE(font_cn_16);
LV_FONT_DECLARE(font_cn_20);
LV_FONT_DECLARE(font_cn_24);

const lv_font_t* ui_font(int size)
{
    if (lang_get() == LANG_CN) {
        switch (size) {
            case 12: return &font_cn_12;
            case 14: return &font_cn_14;
            case 16: return &font_cn_16;
            case 20: return &font_cn_20;
            case 24: return &font_cn_24;
            default: return &font_cn_16;
        }
    }
    switch (size) {
        case 12: return &lv_font_montserrat_12;
        case 14: return &lv_font_montserrat_14;
        case 16: return &lv_font_montserrat_16;
        case 20: return &lv_font_montserrat_20;
        case 24: return &lv_font_montserrat_24;
        case 28: return &lv_font_montserrat_28;
        case 32: return &lv_font_montserrat_32;
        default: return &lv_font_montserrat_16;
    }
}

void ui_refresh_fonts(lv_obj_t *obj)
{
    if (!obj) return;
    if (lv_obj_check_type(obj, &lv_label_class)) {
        int size = (int)(uintptr_t)lv_obj_get_user_data(obj);
        if (size > 0) lv_obj_set_style_text_font(obj, ui_font(size), 0);
    }
    uint32_t cnt = lv_obj_get_child_cnt(obj);
    for (uint32_t i = 0; i < cnt; i++) {
        ui_refresh_fonts(lv_obj_get_child(obj, i));
    }
}

static lv_obj_t *tabview;
static lv_obj_t *tab_dash;
static lv_obj_t *tab_media;
static lv_obj_t *tab_nav;
static lv_obj_t *tab_ctrl;

static void update_timer_cb(lv_timer_t *timer)
{
    (void)timer;
    app_simulate_step();
    ui_dashboard_update();
}

static void disable_scroll_all(lv_obj_t *obj)
{
    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    uint32_t cnt = lv_obj_get_child_cnt(obj);
    for (uint32_t i = 0; i < cnt; i++) {
        disable_scroll_all(lv_obj_get_child(obj, i));
    }
}

static void style_tab(lv_obj_t *tab)
{
    lv_obj_set_style_bg_color(tab, COLOR_BG, 0);
    lv_obj_set_style_bg_opa(tab, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(tab, 0, 0);
    lv_obj_set_style_border_width(tab, 0, 0);
    lv_obj_set_scrollbar_mode(tab, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(tab, LV_OBJ_FLAG_SCROLLABLE);
}

void ui_refresh_language(void)
{
    lv_obj_t *tab_btns = lv_tabview_get_tab_btns(tabview);
    lv_obj_set_style_text_font(tab_btns, ui_font(14), LV_PART_ITEMS);
    lv_tabview_rename_tab(tabview, 0, lang_get_str(STR_TAB_DASH));
    lv_tabview_rename_tab(tabview, 1, lang_get_str(STR_TAB_MEDIA));
    lv_tabview_rename_tab(tabview, 2, lang_get_str(STR_TAB_NAV));
    lv_tabview_rename_tab(tabview, 3, lang_get_str(STR_TAB_CTRL));
    ui_dashboard_refresh_lang();
    ui_statusbar_refresh_lang();
    ui_control_refresh_lang();
    ui_media_refresh_lang();
    ui_nav_refresh_lang();
    ui_refresh_fonts(lv_scr_act());
}

void ui_init(void)
{
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, COLOR_BG, 0);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(scr, 0, 0);
    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_COLUMN);

    ui_create_statusbar(scr);

    tabview = lv_tabview_create(scr, LV_DIR_BOTTOM, 40);
    lv_obj_set_width(tabview, lv_pct(100));
    lv_obj_set_flex_grow(tabview, 1);
    lv_obj_set_style_bg_color(tabview, COLOR_BG, 0);
    lv_obj_set_style_bg_opa(tabview, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(tabview, 0, 0);
    lv_obj_set_style_pad_all(tabview, 0, 0);
    lv_obj_set_scrollbar_mode(tabview, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(tabview, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *tab_btns = lv_tabview_get_tab_btns(tabview);
    lv_obj_set_scrollbar_mode(tab_btns, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(tab_btns, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(tab_btns, COLOR_CARD, 0);
    lv_obj_set_style_bg_opa(tab_btns, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(tab_btns, COLOR_SEC, LV_PART_MAIN);
    lv_obj_set_style_text_color(tab_btns, COLOR_CYAN, LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(tab_btns, COLOR_CARD2, LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(tab_btns, LV_OPA_COVER, LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_style_border_width(tab_btns, 0, LV_PART_ITEMS);
    lv_obj_set_style_radius(tab_btns, 0, LV_PART_ITEMS);
    lv_obj_set_style_pad_top(tab_btns, 4, LV_PART_ITEMS);
    lv_obj_set_style_pad_bottom(tab_btns, 4, LV_PART_ITEMS);
    lv_obj_set_style_text_font(tab_btns, ui_font(14), LV_PART_ITEMS);

    tab_dash = lv_tabview_add_tab(tabview, lang_get_str(STR_TAB_DASH));
    tab_media = lv_tabview_add_tab(tabview, lang_get_str(STR_TAB_MEDIA));
    tab_nav = lv_tabview_add_tab(tabview, lang_get_str(STR_TAB_NAV));
    tab_ctrl = lv_tabview_add_tab(tabview, lang_get_str(STR_TAB_CTRL));

    style_tab(tab_dash);
    style_tab(tab_media);
    style_tab(tab_nav);
    style_tab(tab_ctrl);

    ui_create_dashboard(tab_dash);
    ui_create_media_page(tab_media);
    ui_create_nav_page(tab_nav);
    ui_create_control_page(tab_ctrl);

    disable_scroll_all(lv_scr_act());

    lv_timer_create(update_timer_cb, 100, NULL);
}
