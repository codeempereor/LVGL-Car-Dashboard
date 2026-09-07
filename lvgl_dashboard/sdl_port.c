#include "sdl_port.h"
#include <SDL.h>
#include <string.h>
#include <stdio.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *texture = NULL;
static int32_t disp_hor = 0;
static int32_t disp_ver = 0;

static int16_t mouse_x = 0;
static int16_t mouse_y = 0;
static bool mouse_pressed = false;

static uint32_t key_code = 0;
static bool key_pressed = false;

static volatile bool quit_requested = false;

bool sdl_port_quit_requested(void)
{
    return quit_requested;
}

int sdl_port_init(int32_t hor_res, int32_t ver_res)
{
    disp_hor = hor_res;
    disp_ver = ver_res;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return -1;
    }

    window = SDL_CreateWindow("LVGL Dashboard",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              hor_res, ver_res, SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        return -1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
        return -1;
    }

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                SDL_TEXTUREACCESS_STREAMING, hor_res, ver_res);
    if (!texture) {
        fprintf(stderr, "SDL_CreateTexture failed: %s\n", SDL_GetError());
        return -1;
    }

    return 0;
}

void sdl_port_pump_events(void)
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                quit_requested = true;
                break;
            case SDL_MOUSEMOTION:
                mouse_x = event.motion.x;
                mouse_y = event.motion.y;
                break;
            case SDL_MOUSEBUTTONDOWN:
                mouse_pressed = true;
                break;
            case SDL_MOUSEBUTTONUP:
                mouse_pressed = false;
                break;
            case SDL_KEYDOWN:
                key_pressed = true;
                switch (event.key.keysym.sym) {
                    case SDLK_UP: key_code = LV_KEY_UP; break;
                    case SDLK_DOWN: key_code = LV_KEY_DOWN; break;
                    case SDLK_LEFT: key_code = LV_KEY_LEFT; break;
                    case SDLK_RIGHT: key_code = LV_KEY_RIGHT; break;
                    case SDLK_RETURN: key_code = LV_KEY_ENTER; break;
                    case SDLK_ESCAPE: key_code = LV_KEY_ESC; break;
                    case SDLK_DELETE: key_code = LV_KEY_DEL; break;
                    case SDLK_BACKSPACE: key_code = LV_KEY_BACKSPACE; break;
                    default: key_code = event.key.keysym.sym; break;
                }
                break;
            case SDL_KEYUP:
                key_pressed = false;
                break;
            default:
                break;
        }
    }
}

void sdl_port_display_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = lv_area_get_width(area);
    uint32_t h = lv_area_get_height(area);

    SDL_Rect rect;
    rect.x = area->x1;
    rect.y = area->y1;
    rect.w = (int)w;
    rect.h = (int)h;

    SDL_UpdateTexture(texture, &rect, color_p, (int)(w * 4));
    SDL_RenderCopy(renderer, texture, &rect, &rect);
    SDL_RenderPresent(renderer);

    lv_disp_flush_ready(disp_drv);
}

void sdl_port_mouse_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    (void)indev_drv;
    data->point.x = mouse_x;
    data->point.y = mouse_y;
    data->state = mouse_pressed ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
}

void sdl_port_keyboard_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    (void)indev_drv;
    data->key = key_code;
    data->state = key_pressed ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
}

void sdl_port_deinit(void)
{
    if (texture) SDL_DestroyTexture(texture);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}
