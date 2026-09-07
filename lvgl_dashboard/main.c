#include "lvgl.h"
#include "sdl_port.h"
#include <stdio.h>
#include <SDL.h>

#include "app/app.h"
#include "ui/ui.h"

#define DISP_HOR_RES 1024
#define DISP_VER_RES 600

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf1[DISP_HOR_RES * DISP_VER_RES];
static lv_color_t buf2[DISP_HOR_RES * DISP_VER_RES];

static void hal_init(void)
{
    sdl_port_init(DISP_HOR_RES, DISP_VER_RES);

    lv_disp_draw_buf_init(&draw_buf, buf1, buf2, DISP_HOR_RES * DISP_VER_RES);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = DISP_HOR_RES;
    disp_drv.ver_res = DISP_VER_RES;
    disp_drv.flush_cb = sdl_port_display_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = sdl_port_mouse_read;
    lv_indev_drv_register(&indev_drv);

    static lv_indev_drv_t kb_drv;
    lv_indev_drv_init(&kb_drv);
    kb_drv.type = LV_INDEV_TYPE_KEYPAD;
    kb_drv.read_cb = sdl_port_keyboard_read;
    lv_indev_drv_register(&kb_drv);
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    lv_init();
    hal_init();

    app_init();
    ui_init();

    printf("LVGL Dashboard started. Close window to exit.\n");

    uint32_t last_tick = SDL_GetTicks();

    while (!sdl_port_quit_requested()) {
        uint32_t now = SDL_GetTicks();
        uint32_t elapsed = now - last_tick;
        if (elapsed > 0) {
            lv_tick_inc(elapsed);
            last_tick = now;
        }

        sdl_port_pump_events();
        lv_timer_handler();
        SDL_Delay(5);
    }

    sdl_port_deinit();
    printf("LVGL Dashboard exited cleanly.\n");

    return 0;
}
