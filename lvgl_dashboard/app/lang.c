#include "lang.h"

static LangType current_lang = LANG_EN;

static const char *str_table[STR_COUNT][LANG_COUNT] = {
    [STR_TAB_DASH]          = {"Dashboard", "仪表盘"},
    [STR_TAB_MEDIA]         = {"Media", "媒体"},
    [STR_TAB_NAV]           = {"Navigation", "导航"},
    [STR_TAB_CTRL]          = {"Control", "控制"},
    [STR_VEHICLE_STATUS]    = {"VEHICLE STATUS", "车辆状态"},
    [STR_INSTANT_CONS]      = {"Instant Cons.", "瞬时油耗"},
    [STR_RANGE]             = {"Range", "续航里程"},
    [STR_OUTDOOR_TEMP]      = {"Outdoor Temp", "室外温度"},
    [STR_FUEL]              = {"FUEL", "油量"},
    [STR_COOLANT]           = {"COOLANT", "水温"},
    [STR_BATTERY]           = {"BATTERY", "电压"},
    [STR_ODOMETER]          = {"ODOMETER", "总里程"},
    [STR_AVG_CONS]          = {"AVG CONS", "平均油耗"},
    [STR_NOW_PLAYING]       = {"NOW PLAYING", "正在播放"},
    [STR_ETA]               = {"ETA", "预计到达"},
    [STR_REMAINING]         = {"REMAINING", "剩余路程"},
    [STR_SPEED_LIMIT]       = {"SPEED LIMIT", "限速"},
    [STR_LANE]              = {"LANE", "车道"},
    [STR_ROUTE_PROGRESS]    = {"ROUTE PROGRESS", "路线进度"},
    [STR_TRANSMISSION]      = {"TRANSMISSION", "变速箱"},
    [STR_LIGHTING]          = {"LIGHTING", "灯光控制"},
    [STR_LEFT_TURN]         = {"Left Turn", "左转向灯"},
    [STR_HEADLIGHT]         = {"Headlight", "大灯"},
    [STR_RIGHT_TURN]        = {"Right Turn", "右转向灯"},
    [STR_DISPLAY_BRIGHTNESS] = {"DISPLAY BRIGHTNESS", "屏幕亮度"},
    [STR_CLIMATE_CONTROL]   = {"CLIMATE CONTROL", "空调控制"},
    [STR_AC_ON]             = {"A/C ON", "空调开"},
    [STR_AC_OFF]            = {"A/C OFF", "空调关"},
    [STR_LANGUAGE]          = {"Language", "语言"},
    [STR_DESTINATION]       = {"Destination", "目的地"},
};

void lang_set(LangType lang)
{
    if (lang < LANG_COUNT) current_lang = lang;
}

LangType lang_get(void)
{
    return current_lang;
}

void lang_toggle(void)
{
    current_lang = (current_lang == LANG_EN) ? LANG_CN : LANG_EN;
}

const char* lang_get_str(StrId id)
{
    if (id >= STR_COUNT) return "";
    return str_table[id][current_lang];
}
