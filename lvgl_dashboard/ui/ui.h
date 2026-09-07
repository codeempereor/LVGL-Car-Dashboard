#ifndef UI_H
#define UI_H

#include "lvgl.h"
#include "app/lang.h"

const lv_font_t* ui_font(int size);
void ui_refresh_fonts(lv_obj_t *obj);

#define COLOR_BG       lv_color_hex(0x000000)
#define COLOR_CARD     lv_color_hex(0x161616)
#define COLOR_CARD2    lv_color_hex(0x2A2A2A)
#define COLOR_CYAN     lv_color_hex(0x00E5FF)
#define COLOR_BLUE     lv_color_hex(0x40C4FF)
#define COLOR_AMBER    lv_color_hex(0xFFB300)
#define COLOR_RED      lv_color_hex(0xFF5252)
#define COLOR_GREEN    lv_color_hex(0x69F0AE)
#define COLOR_TEXT     lv_color_hex(0xFFFFFF)
#define COLOR_SEC      lv_color_hex(0xB0BEC5)
#define COLOR_MUTED    lv_color_hex(0x607D8B)

void ui_init(void);
void ui_refresh_language(void);

lv_obj_t* ui_create_dashboard(lv_obj_t* parent);
void ui_dashboard_update(void);
void ui_dashboard_refresh_lang(void);

lv_obj_t* ui_create_statusbar(lv_obj_t* parent);
void ui_statusbar_refresh_lang(void);

lv_obj_t* ui_create_control_page(lv_obj_t* parent);
void ui_control_refresh_lang(void);

lv_obj_t* ui_create_media_page(lv_obj_t* parent);
void ui_media_refresh_lang(void);

lv_obj_t* ui_create_nav_page(lv_obj_t* parent);
void ui_nav_refresh_lang(void);

#endif
