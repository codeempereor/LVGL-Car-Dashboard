#include "lvgl_port.h"
#include "st7789v.h"
#include "ui/ui_mini.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

static const char *TAG = "LVGL_PORT";

#define BUF_LINES 40
#define BUF_SIZE (ST7789V_WIDTH * BUF_LINES)

static uint16_t buf1[BUF_SIZE];
static uint16_t buf2[BUF_SIZE];

static lv_disp_t *disp = NULL;
static TaskHandle_t lvgl_task_handle = NULL;
static SemaphoreHandle_t lvgl_mux = NULL;

static void flush_cb(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    st7789v_flush(area->x1, area->y1, area->x2, area->y2, (const uint16_t *)color_p);
    lv_disp_flush_ready(disp_drv);
}

static void lvgl_task(void *arg)
{
    uint32_t tick = 0;
    ESP_LOGI(TAG, "lvgl_task running");
    while (1) {
        if (xSemaphoreTake(lvgl_mux, portMAX_DELAY) == pdTRUE) {
            lv_tick_inc(5);
            tick++;
            if (tick % 40 == 0) {
                ui_mini_update();
            }
            lv_task_handler();
            xSemaphoreGive(lvgl_mux);
        }
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

void lvgl_port_init(void)
{
    lv_init();

    static lv_disp_draw_buf_t draw_buf;
    lv_disp_draw_buf_init(&draw_buf, buf1, buf2, BUF_SIZE);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = ST7789V_WIDTH;
    disp_drv.ver_res = ST7789V_HEIGHT;
    disp_drv.flush_cb = flush_cb;
    disp_drv.draw_buf = &draw_buf;
    disp_drv.full_refresh = 0;

    disp = lv_disp_drv_register(&disp_drv);

    lvgl_mux = xSemaphoreCreateMutex();

    ESP_LOGI(TAG, "LVGL initialized (%dx%d, buf_lines=%d)",
             ST7789V_WIDTH, ST7789V_HEIGHT, BUF_LINES);
}

void lvgl_port_start(void)
{
    if (lvgl_task_handle != NULL) return;

    xTaskCreatePinnedToCore(
        lvgl_task,
        "lvgl",
        4096,
        NULL,
        5,
        &lvgl_task_handle,
        1
    );

    ESP_LOGI(TAG, "LVGL task started on core 1");
}

void lvgl_port_deinit(void)
{
    if (lvgl_task_handle) {
        vTaskDelete(lvgl_task_handle);
        lvgl_task_handle = NULL;
    }
    if (lvgl_mux) {
        vSemaphoreDelete(lvgl_mux);
        lvgl_mux = NULL;
    }
}

lv_disp_t *lvgl_port_get_disp(void)
{
    return disp;
}
