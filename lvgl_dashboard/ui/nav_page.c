#include "ui.h"
#include "app/lang.h"
#include <stdio.h>

static lv_obj_t *nav_direction;
static lv_obj_t *nav_street;
static lv_obj_t *nav_distance;
static lv_obj_t *nav_eta;
static lv_obj_t *nav_remaining;
static lv_obj_t *route_bar;
static lv_obj_t *eta_title, *rem_title, *speed_title, *lane_title;
static lv_obj_t *dest_label;
static lv_obj_t *route_title;
static int route_progress = 35;
static int nav_step = 0;

static const char *directions[] = {
    LV_SYMBOL_RIGHT, LV_SYMBOL_UP, LV_SYMBOL_LEFT, LV_SYMBOL_UP,
    LV_SYMBOL_RIGHT, LV_SYMBOL_DOWN, LV_SYMBOL_LEFT, LV_SYMBOL_UP
};
static const char *streets_en[] = {
    "Turn right onto Main St",
    "Continue straight on 5th Ave",
    "Turn left onto Park Rd",
    "Keep straight on Highway 101",
    "Turn right onto Elm St",
    "Exit left toward Downtown",
    "Turn left onto Oak Ave",
    "Arrive at destination"
};
static const char *streets_cn[] = {
    "右转进入主街",
    "沿第五大道直行",
    "左转进入公园路",
    "沿101号高速直行",
    "右转进入榆树街",
    "左侧出口驶向市中心",
    "左转进入橡树大道",
    "到达目的地"
};
static const char *lane_en[] = {"Keep left", "Keep right", "Stay straight", "Merge left"};
static const char *lane_cn[] = {"保持左侧", "保持右侧", "保持直行", "向左并线"};

static void nav_timer_cb(lv_timer_t *timer)
{
    (void)timer;
    route_progress++;
    if (route_progress > 100) route_progress = 0;
    lv_bar_set_value(route_bar, route_progress, LV_ANIM_ON);

    if (route_progress % 20 == 0) {
        nav_step = (nav_step + 1) % 8;
        const char **streets = (lang_get() == LANG_CN) ? streets_cn : streets_en;
        lv_label_set_text(nav_direction, directions[nav_step]);
        lv_label_set_text(nav_street, streets[nav_step]);
    }

    int dist = 1200 - route_progress * 10;
    if (dist < 0) dist = 0;
    char buf[32];
    if (dist >= 1000) snprintf(buf, sizeof(buf), "%.1f km", dist / 1000.0);
    else snprintf(buf, sizeof(buf), "%d m", dist);
    lv_label_set_text(nav_distance, buf);

    int eta_min = 15 - route_progress / 10;
    if (eta_min < 1) eta_min = 1;
    snprintf(buf, sizeof(buf), "%d min", eta_min);
    lv_label_set_text(nav_eta, buf);

    snprintf(buf, sizeof(buf), "%.1f km", 12.5 - route_progress * 0.12);
    lv_label_set_text(nav_remaining, buf);
}

static lv_obj_t* make_info_card(lv_obj_t *parent, lv_obj_t **title_out, StrId title_id, int x, int y)
{
    lv_obj_t *card = lv_obj_create(parent);
    lv_obj_set_size(card, 200, 80);
    lv_obj_set_pos(card, x, y);
    lv_obj_set_style_bg_color(card, COLOR_CARD, 0);
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(card, COLOR_CARD2, 0);
    lv_obj_set_style_border_width(card, 1, 0);
    lv_obj_set_style_radius(card, 10, 0);
    lv_obj_set_style_pad_all(card, 12, 0);

    *title_out = lv_label_create(card);
    lv_label_set_text(*title_out, lang_get_str(title_id));
    lv_obj_set_style_text_color(*title_out, COLOR_MUTED, 0);
    lv_obj_set_style_text_font(*title_out, ui_font(12), 0); lv_obj_set_user_data(*title_out, (void*)(uintptr_t)12);
    lv_obj_align(*title_out, LV_ALIGN_TOP_LEFT, 0, 0);

    return card;
}

void ui_nav_refresh_lang(void)
{
    const char **streets = (lang_get() == LANG_CN) ? streets_cn : streets_en;
    const char **lanes = (lang_get() == LANG_CN) ? lane_cn : lane_en;
    lv_label_set_text(nav_street, streets[nav_step]);
    lv_label_set_text(dest_label, lang_get_str(STR_DESTINATION));
    lv_label_set_text(eta_title, lang_get_str(STR_ETA));
    lv_label_set_text(rem_title, lang_get_str(STR_REMAINING));
    lv_label_set_text(speed_title, lang_get_str(STR_SPEED_LIMIT));
    lv_label_set_text(lane_title, lang_get_str(STR_LANE));
    lv_label_set_text(route_title, lang_get_str(STR_ROUTE_PROGRESS));
}

lv_obj_t* ui_create_nav_page(lv_obj_t* parent)
{
    lv_obj_t *top_panel = lv_obj_create(parent);
    lv_obj_set_size(top_panel, 944, 200);
    lv_obj_set_pos(top_panel, 40, 20);
    lv_obj_set_style_bg_color(top_panel, COLOR_CARD, 0);
    lv_obj_set_style_bg_opa(top_panel, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(top_panel, COLOR_CARD2, 0);
    lv_obj_set_style_border_width(top_panel, 1, 0);
    lv_obj_set_style_radius(top_panel, 12, 0);
    lv_obj_set_style_pad_all(top_panel, 0, 0);

    nav_direction = lv_label_create(top_panel);
    lv_label_set_text(nav_direction, LV_SYMBOL_RIGHT);
    lv_obj_set_style_text_color(nav_direction, COLOR_CYAN, 0);
    lv_obj_set_style_text_font(nav_direction, ui_font(32), 0); lv_obj_set_user_data(nav_direction, (void*)(uintptr_t)32);
    lv_obj_align(nav_direction, LV_ALIGN_LEFT_MID, 50, 0);

    lv_obj_t *right_col = lv_obj_create(top_panel);
    lv_obj_set_size(right_col, 600, LV_SIZE_CONTENT);
    lv_obj_align(right_col, LV_ALIGN_RIGHT_MID, -30, 0);
    lv_obj_set_style_bg_opa(right_col, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(right_col, 0, 0);
    lv_obj_set_style_pad_all(right_col, 0, 0);
    lv_obj_set_flex_flow(right_col, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_gap(right_col, 8, 0);

    nav_street = lv_label_create(right_col);
    lv_label_set_text(nav_street, streets_en[0]);
    lv_obj_set_style_text_color(nav_street, COLOR_TEXT, 0);
    lv_obj_set_style_text_font(nav_street, ui_font(24), 0); lv_obj_set_user_data(nav_street, (void*)(uintptr_t)24);

    nav_distance = lv_label_create(right_col);
    lv_label_set_text(nav_distance, "1.2 km");
    lv_obj_set_style_text_color(nav_distance, COLOR_CYAN, 0);
    lv_obj_set_style_text_font(nav_distance, ui_font(20), 0); lv_obj_set_user_data(nav_distance, (void*)(uintptr_t)20);

    dest_label = lv_label_create(right_col);
    lv_label_set_text(dest_label, lang_get_str(STR_DESTINATION));
    lv_obj_set_style_text_color(dest_label, COLOR_MUTED, 0);
    lv_obj_set_style_text_font(dest_label, ui_font(14), 0); lv_obj_set_user_data(dest_label, (void*)(uintptr_t)14);

    int card_w = 200, card_gap = 48;
    int total_w = 4 * card_w + 3 * card_gap;
    int start_x = (1024 - total_w) / 2;
    int card_y = 246;

    lv_obj_t *eta_card = make_info_card(parent, &eta_title, STR_ETA, start_x, card_y);
    nav_eta = lv_label_create(eta_card);
    lv_label_set_text(nav_eta, "12 min");
    lv_obj_set_style_text_color(nav_eta, COLOR_GREEN, 0);
    lv_obj_set_style_text_font(nav_eta, ui_font(20), 0); lv_obj_set_user_data(nav_eta, (void*)(uintptr_t)20);
    lv_obj_align(nav_eta, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    lv_obj_t *rem_card = make_info_card(parent, &rem_title, STR_REMAINING, start_x + card_w + card_gap, card_y);
    nav_remaining = lv_label_create(rem_card);
    lv_label_set_text(nav_remaining, "8.3 km");
    lv_obj_set_style_text_color(nav_remaining, COLOR_CYAN, 0);
    lv_obj_set_style_text_font(nav_remaining, ui_font(20), 0); lv_obj_set_user_data(nav_remaining, (void*)(uintptr_t)20);
    lv_obj_align(nav_remaining, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    lv_obj_t *speed_card = make_info_card(parent, &speed_title, STR_SPEED_LIMIT, start_x + 2*(card_w + card_gap), card_y);
    lv_obj_t *speed_limit = lv_label_create(speed_card);
    lv_label_set_text(speed_limit, "60 km/h");
    lv_obj_set_style_text_color(speed_limit, COLOR_AMBER, 0);
    lv_obj_set_style_text_font(speed_limit, ui_font(20), 0); lv_obj_set_user_data(speed_limit, (void*)(uintptr_t)20);
    lv_obj_align(speed_limit, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    lv_obj_t *lane_card = make_info_card(parent, &lane_title, STR_LANE, start_x + 3*(card_w + card_gap), card_y);
    const char **lanes = (lang_get() == LANG_CN) ? lane_cn : lane_en;
    lv_obj_t *lane_info = lv_label_create(lane_card);
    lv_label_set_text(lane_info, lanes[0]);
    lv_obj_set_style_text_color(lane_info, COLOR_BLUE, 0);
    lv_obj_set_style_text_font(lane_info, ui_font(20), 0); lv_obj_set_user_data(lane_info, (void*)(uintptr_t)20);
    lv_obj_align(lane_info, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    route_title = lv_label_create(parent);
    lv_label_set_text(route_title, lang_get_str(STR_ROUTE_PROGRESS));
    lv_obj_set_style_text_color(route_title, COLOR_SEC, 0);
    lv_obj_set_style_text_font(route_title, ui_font(14), 0); lv_obj_set_user_data(route_title, (void*)(uintptr_t)14);
    lv_obj_align(route_title, LV_ALIGN_BOTTOM_LEFT, 40, -60);

    route_bar = lv_bar_create(parent);
    lv_obj_set_size(route_bar, 944, 16);
    lv_bar_set_range(route_bar, 0, 100);
    lv_bar_set_value(route_bar, route_progress, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(route_bar, COLOR_CARD2, LV_PART_MAIN);
    lv_obj_set_style_bg_color(route_bar, COLOR_CYAN, LV_PART_INDICATOR);
    lv_obj_set_style_radius(route_bar, 8, LV_PART_MAIN);
    lv_obj_set_style_radius(route_bar, 8, LV_PART_INDICATOR);
    lv_obj_align(route_bar, LV_ALIGN_BOTTOM_MID, 0, -30);

    lv_timer_create(nav_timer_cb, 800, NULL);

    return parent;
}
