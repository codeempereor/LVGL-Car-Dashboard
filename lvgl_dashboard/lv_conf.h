#ifndef LV_CONF_H
#define LV_CONF_H

#define LV_COLOR_DEPTH 32
#define LV_DPI_DEF 130
#define LV_MEM_SIZE (128U * 1024U)
#define LV_DISP_DEF_REFR_PERIOD 16
#define LV_DRAW_COMPLEX 1

#define LV_USE_THEME_DEFAULT 1
#define LV_THEME_DEFAULT_COLOR_PRIMARY lv_color_hex(0x2196F3)
#define LV_THEME_DEFAULT_FONT_SMALL &lv_font_montserrat_14
#define LV_THEME_DEFAULT_FONT_NORMAL &lv_font_montserrat_16
#define LV_THEME_DEFAULT_FONT_SUBTITLE &lv_font_montserrat_20
#define LV_THEME_DEFAULT_FONT_TITLE &lv_font_montserrat_24

#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_20 1
#define LV_FONT_MONTSERRAT_24 1
#define LV_FONT_MONTSERRAT_28 1
#define LV_FONT_MONTSERRAT_32 1
#define LV_FONT_DEFAULT &lv_font_montserrat_16

#define LV_USE_METER 1
#define LV_USE_ARC 1
#define LV_USE_BAR 1
#define LV_USE_CHART 1
#define LV_USE_SLIDER 1
#define LV_USE_SWITCH 1
#define LV_USE_BTN 1
#define LV_USE_LABEL 1
#define LV_USE_DROPDOWN 1
#define LV_USE_ROLLER 1
#define LV_USE_TABVIEW 1
#define LV_USE_TILEVIEW 1
#define LV_USE_LIST 1
#define LV_USE_IMG 1
#define LV_USE_LED 1
#define LV_USE_FLEX 1
#define LV_USE_GRID 1
#define LV_USE_ANIMATION 1
#define LV_USE_LINEMETER 1

#define LV_USE_DEMO_WIDGETS 0
#define LV_USE_DEMO_BENCHMARK 0
#define LV_USE_LOG 0
#define LV_USE_PERF_MONITOR 0
#define LV_USE_MEM_MONITOR 0

#endif
