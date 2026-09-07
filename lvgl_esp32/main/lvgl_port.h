#ifndef LVGL_PORT_H
#define LVGL_PORT_H

#include "lvgl.h"

void lvgl_port_init(void);
void lvgl_port_start(void);
void lvgl_port_deinit(void);
lv_disp_t *lvgl_port_get_disp(void);

#endif
