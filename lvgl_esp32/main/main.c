#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "st7789v.h"
#include "lvgl_port.h"
#include "ui/ui_mini.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    ESP_LOGI(TAG, "Starting LVGL ESP32-S3 dashboard...");

    st7789v_init();
    lvgl_port_init();
    ui_mini_init();

    lvgl_port_start();

    ESP_LOGI(TAG, "Dashboard running");

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
