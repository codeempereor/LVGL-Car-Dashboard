#ifndef SDL_PORT_H
#define SDL_PORT_H

#include "lvgl.h"
#include <stdbool.h>

int  sdl_port_init(int32_t hor_res, int32_t ver_res);
void sdl_port_deinit(void);
bool sdl_port_quit_requested(void);
void sdl_port_pump_events(void);

void sdl_port_display_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);
void sdl_port_mouse_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data);
void sdl_port_keyboard_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data);

#endif
