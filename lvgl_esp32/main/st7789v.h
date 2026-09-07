#ifndef ST7789V_H
#define ST7789V_H

#include <stdint.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"

#define ST7789V_WIDTH  320
#define ST7789V_HEIGHT 240

#define ST7789V_PIN_SCLK   GPIO_NUM_9
#define ST7789V_PIN_MOSI   GPIO_NUM_10
#define ST7789V_PIN_MISO   -1
#define ST7789V_PIN_CS     GPIO_NUM_14
#define ST7789V_PIN_DC     GPIO_NUM_13
#define ST7789V_PIN_RST    GPIO_NUM_11
#define ST7789V_PIN_BL     GPIO_NUM_8

#define ST7789V_SPI_HOST   SPI2_HOST
#define ST7789V_SPI_FREQ   (3 * 1000 * 1000)
#define ST7789V_SPI_MODE   0

void st7789v_init(void);
void st7789v_deinit(void);
void st7789v_set_backlight(uint8_t on);
void st7789v_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void st7789v_write_pixels(const uint16_t *buf, uint32_t len);
void st7789v_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const uint16_t *buf);
void st7789v_fill(uint16_t color);

#endif
