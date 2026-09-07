#ifndef LANG_H
#define LANG_H

#include <stdint.h>

typedef enum {
    LANG_EN = 0,
    LANG_CN,
    LANG_COUNT
} LangType;

typedef enum {
    STR_TAB_DASH = 0,
    STR_TAB_MEDIA,
    STR_TAB_NAV,
    STR_TAB_CTRL,
    STR_VEHICLE_STATUS,
    STR_INSTANT_CONS,
    STR_RANGE,
    STR_OUTDOOR_TEMP,
    STR_FUEL,
    STR_COOLANT,
    STR_BATTERY,
    STR_ODOMETER,
    STR_AVG_CONS,
    STR_NOW_PLAYING,
    STR_ETA,
    STR_REMAINING,
    STR_SPEED_LIMIT,
    STR_LANE,
    STR_ROUTE_PROGRESS,
    STR_TRANSMISSION,
    STR_LIGHTING,
    STR_LEFT_TURN,
    STR_HEADLIGHT,
    STR_RIGHT_TURN,
    STR_DISPLAY_BRIGHTNESS,
    STR_CLIMATE_CONTROL,
    STR_AC_ON,
    STR_AC_OFF,
    STR_LANGUAGE,
    STR_DESTINATION,
    STR_COUNT
} StrId;

void lang_set(LangType lang);
LangType lang_get(void);
void lang_toggle(void);
const char* lang_get_str(StrId id);

#endif
