#include "ui.h"
#include "app/lang.h"
#include <stdio.h>

static lv_obj_t *song_title;
static lv_obj_t *song_artist;
static lv_obj_t *progress_bar;
static lv_obj_t *progress_time;
static lv_obj_t *play_btn_label;
static lv_obj_t *now_playing_label;
static bool playing = true;
static int song_idx = 0;
static uint32_t song_pos = 45;

static const char *songs_en[][2] = {
    {"Midnight Drive", "Synthwave Collective"},
    {"Neon Horizon", "Electric Dreams"},
    {"Turbo Boost", "Retro Racer"},
    {"Ocean Breeze", "Chill Vibes"},
    {"City Lights", "Urban Pulse"},
};
static const char *songs_cn[][2] = {
    {"夜驰", "合成波乐队"},
    {"霓虹天际", "电子梦境"},
    {"涡轮增压", "复古赛车"},
    {"海风", "轻松氛围"},
    {"城市灯光", "都市脉冲"},
};

static void update_song_info(void)
{
    if (lang_get() == LANG_CN) {
        lv_label_set_text(song_title, songs_cn[song_idx][0]);
        lv_label_set_text(song_artist, songs_cn[song_idx][1]);
    } else {
        lv_label_set_text(song_title, songs_en[song_idx][0]);
        lv_label_set_text(song_artist, songs_en[song_idx][1]);
    }
    song_pos = 0;
    lv_bar_set_value(progress_bar, 0, LV_ANIM_OFF);
}

static void play_pause_cb(lv_event_t *e)
{
    (void)e;
    playing = !playing;
    lv_label_set_text(play_btn_label, playing ? LV_SYMBOL_PAUSE : LV_SYMBOL_PLAY);
}

static void prev_cb(lv_event_t *e)
{
    (void)e;
    song_idx = (song_idx - 1 + 5) % 5;
    update_song_info();
}

static void next_cb(lv_event_t *e)
{
    (void)e;
    song_idx = (song_idx + 1) % 5;
    update_song_info();
}

static void media_timer_cb(lv_timer_t *timer)
{
    (void)timer;
    if (!playing) return;
    song_pos++;
    if (song_pos > 200) song_pos = 0;
    lv_bar_set_value(progress_bar, song_pos, LV_ANIM_OFF);
    int min = song_pos / 60;
    int sec = song_pos % 60;
    char buf[24];
    snprintf(buf, sizeof(buf), "%d:%02d / 3:20", min, sec);
    lv_label_set_text(progress_time, buf);
}

void ui_media_refresh_lang(void)
{
    lv_label_set_text(now_playing_label, lang_get_str(STR_NOW_PLAYING));
    update_song_info();
}

lv_obj_t* ui_create_media_page(lv_obj_t* parent)
{
    lv_obj_t *left_panel = lv_obj_create(parent);
    lv_obj_set_size(left_panel, 340, 460);
    lv_obj_set_pos(left_panel, 40, 28);
    lv_obj_set_style_bg_color(left_panel, COLOR_CARD, 0);
    lv_obj_set_style_bg_opa(left_panel, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(left_panel, COLOR_CARD2, 0);
    lv_obj_set_style_border_width(left_panel, 1, 0);
    lv_obj_set_style_radius(left_panel, 12, 0);
    lv_obj_set_style_pad_all(left_panel, 0, 0);

    lv_obj_t *album = lv_obj_create(left_panel);
    lv_obj_set_size(album, 220, 220);
    lv_obj_align(album, LV_ALIGN_TOP_MID, 0, 40);
    lv_obj_set_style_bg_color(album, COLOR_CARD2, 0);
    lv_obj_set_style_bg_opa(album, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(album, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_color(album, COLOR_CYAN, 0);
    lv_obj_set_style_border_width(album, 4, 0);
    lv_obj_set_style_pad_all(album, 0, 0);

    lv_obj_t *album_icon = lv_label_create(album);
    lv_label_set_text(album_icon, LV_SYMBOL_AUDIO);
    lv_obj_set_style_text_color(album_icon, COLOR_CYAN, 0);
    lv_obj_set_style_text_font(album_icon, ui_font(32), 0); lv_obj_set_user_data(album_icon, (void*)(uintptr_t)32);
    lv_obj_center(album_icon);

    now_playing_label = lv_label_create(left_panel);
    lv_label_set_text(now_playing_label, lang_get_str(STR_NOW_PLAYING));
    lv_obj_set_style_text_color(now_playing_label, COLOR_MUTED, 0);
    lv_obj_set_style_text_font(now_playing_label, ui_font(12), 0); lv_obj_set_user_data(now_playing_label, (void*)(uintptr_t)12);
    lv_obj_align(now_playing_label, LV_ALIGN_BOTTOM_MID, 0, -60);

    lv_obj_t *right_panel = lv_obj_create(parent);
    lv_obj_set_size(right_panel, 580, 460);
    lv_obj_set_pos(right_panel, 400, 28);
    lv_obj_set_style_bg_color(right_panel, COLOR_CARD, 0);
    lv_obj_set_style_bg_opa(right_panel, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(right_panel, COLOR_CARD2, 0);
    lv_obj_set_style_border_width(right_panel, 1, 0);
    lv_obj_set_style_radius(right_panel, 12, 0);
    lv_obj_set_style_pad_all(right_panel, 30, 0);
    lv_obj_set_flex_flow(right_panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_gap(right_panel, 16, 0);

    song_title = lv_label_create(right_panel);
    lv_label_set_text(song_title, songs_en[0][0]);
    lv_obj_set_style_text_color(song_title, COLOR_TEXT, 0);
    lv_obj_set_style_text_font(song_title, ui_font(28), 0); lv_obj_set_user_data(song_title, (void*)(uintptr_t)28);

    song_artist = lv_label_create(right_panel);
    lv_label_set_text(song_artist, songs_en[0][1]);
    lv_obj_set_style_text_color(song_artist, COLOR_SEC, 0);
    lv_obj_set_style_text_font(song_artist, ui_font(16), 0); lv_obj_set_user_data(song_artist, (void*)(uintptr_t)16);

    lv_obj_t *spacer1 = lv_obj_create(right_panel);
    lv_obj_set_size(spacer1, 1, 20);
    lv_obj_set_style_bg_opa(spacer1, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(spacer1, 0, 0);

    progress_bar = lv_bar_create(right_panel);
    lv_obj_set_width(progress_bar, lv_pct(100));
    lv_obj_set_height(progress_bar, 10);
    lv_bar_set_range(progress_bar, 0, 200);
    lv_bar_set_value(progress_bar, 45, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(progress_bar, COLOR_CARD2, LV_PART_MAIN);
    lv_obj_set_style_bg_color(progress_bar, COLOR_CYAN, LV_PART_INDICATOR);
    lv_obj_set_style_radius(progress_bar, 5, LV_PART_MAIN);
    lv_obj_set_style_radius(progress_bar, 5, LV_PART_INDICATOR);

    progress_time = lv_label_create(right_panel);
    lv_label_set_text(progress_time, "0:45 / 3:20");
    lv_obj_set_style_text_color(progress_time, COLOR_MUTED, 0);
    lv_obj_set_style_text_font(progress_time, ui_font(14), 0); lv_obj_set_user_data(progress_time, (void*)(uintptr_t)14);

    lv_obj_t *controls = lv_obj_create(right_panel);
    lv_obj_set_size(controls, lv_pct(100), 80);
    lv_obj_set_style_bg_opa(controls, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(controls, 0, 0);
    lv_obj_set_style_pad_all(controls, 0, 0);
    lv_obj_set_flex_flow(controls, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(controls, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *prev_btn = lv_btn_create(controls);
    lv_obj_set_size(prev_btn, 60, 60);
    lv_obj_set_style_bg_color(prev_btn, COLOR_CARD2, 0);
    lv_obj_set_style_bg_color(prev_btn, COLOR_CYAN, LV_STATE_PRESSED);
    lv_obj_set_style_radius(prev_btn, LV_RADIUS_CIRCLE, 0);
    lv_obj_t *pl = lv_label_create(prev_btn);
    lv_label_set_text(pl, LV_SYMBOL_PREV);
    lv_obj_set_style_text_color(pl, COLOR_TEXT, 0);
    lv_obj_set_style_text_font(pl, ui_font(24), 0); lv_obj_set_user_data(pl, (void*)(uintptr_t)24);
    lv_obj_center(pl);
    lv_obj_add_event_cb(prev_btn, prev_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *play_btn = lv_btn_create(controls);
    lv_obj_set_size(play_btn, 72, 72);
    lv_obj_set_style_bg_color(play_btn, COLOR_CYAN, 0);
    lv_obj_set_style_bg_color(play_btn, COLOR_BLUE, LV_STATE_PRESSED);
    lv_obj_set_style_radius(play_btn, LV_RADIUS_CIRCLE, 0);
    play_btn_label = lv_label_create(play_btn);
    lv_label_set_text(play_btn_label, LV_SYMBOL_PAUSE);
    lv_obj_set_style_text_color(play_btn_label, COLOR_BG, 0);
    lv_obj_set_style_text_font(play_btn_label, ui_font(28), 0); lv_obj_set_user_data(play_btn_label, (void*)(uintptr_t)28);
    lv_obj_center(play_btn_label);
    lv_obj_add_event_cb(play_btn, play_pause_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *next_btn = lv_btn_create(controls);
    lv_obj_set_size(next_btn, 60, 60);
    lv_obj_set_style_bg_color(next_btn, COLOR_CARD2, 0);
    lv_obj_set_style_bg_color(next_btn, COLOR_CYAN, LV_STATE_PRESSED);
    lv_obj_set_style_radius(next_btn, LV_RADIUS_CIRCLE, 0);
    lv_obj_t *nl = lv_label_create(next_btn);
    lv_label_set_text(nl, LV_SYMBOL_NEXT);
    lv_obj_set_style_text_color(nl, COLOR_TEXT, 0);
    lv_obj_set_style_text_font(nl, ui_font(24), 0); lv_obj_set_user_data(nl, (void*)(uintptr_t)24);
    lv_obj_center(nl);
    lv_obj_add_event_cb(next_btn, next_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *vol_row = lv_obj_create(right_panel);
    lv_obj_set_size(vol_row, lv_pct(100), 40);
    lv_obj_set_style_bg_opa(vol_row, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(vol_row, 0, 0);
    lv_obj_set_style_pad_all(vol_row, 0, 0);
    lv_obj_set_flex_flow(vol_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(vol_row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(vol_row, 12, 0);

    lv_obj_t *vol_icon = lv_label_create(vol_row);
    lv_label_set_text(vol_icon, LV_SYMBOL_VOLUME_MAX);
    lv_obj_set_style_text_color(vol_icon, COLOR_SEC, 0);
    lv_obj_set_style_text_font(vol_icon, ui_font(16), 0); lv_obj_set_user_data(vol_icon, (void*)(uintptr_t)16);

    lv_obj_t *vol_slider = lv_slider_create(vol_row);
    lv_slider_set_range(vol_slider, 0, 100);
    lv_slider_set_value(vol_slider, 65, LV_ANIM_OFF);
    lv_obj_set_width(vol_slider, 300);
    lv_obj_set_height(vol_slider, 10);
    lv_obj_set_style_bg_color(vol_slider, COLOR_CARD2, LV_PART_MAIN);
    lv_obj_set_style_bg_color(vol_slider, COLOR_CYAN, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(vol_slider, COLOR_TEXT, LV_PART_KNOB);

    lv_timer_create(media_timer_cb, 1000, NULL);

    return parent;
}
