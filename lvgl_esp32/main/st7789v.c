#include "st7789v.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

static const char *TAG = "ST7789V";

static spi_device_handle_t spi;
static bool is_init = false;

static void st7789v_delay_ms(uint32_t ms)
{
    vTaskDelay(pdMS_TO_TICKS(ms));
}

static void st7789v_write_cmd(uint8_t cmd)
{
    gpio_set_level(ST7789V_PIN_DC, 0);
    spi_transaction_t t = {0};
    t.flags = SPI_TRANS_USE_TXDATA;
    t.length = 8;
    t.tx_data[0] = cmd;
    spi_device_polling_transmit(spi, &t);
}

static void st7789v_write_data_byte(uint8_t data)
{
    gpio_set_level(ST7789V_PIN_DC, 1);
    spi_transaction_t t = {0};
    t.flags = SPI_TRANS_USE_TXDATA;
    t.length = 8;
    t.tx_data[0] = data;
    spi_device_polling_transmit(spi, &t);
}

static void st7789v_write_data(const uint8_t *data, uint32_t len)
{
    if (len == 0) return;
    gpio_set_level(ST7789V_PIN_DC, 1);
    spi_transaction_t t = {0};
    t.length = len * 8;
    t.tx_buffer = data;
    spi_device_polling_transmit(spi, &t);
}

void st7789v_init(void)
{
    if (is_init) return;

    ESP_LOGI(TAG, "Initializing ST7789V...");

    gpio_config_t io_conf = {0};
    io_conf.pin_bit_mask = (1ULL << ST7789V_PIN_DC) |
                           (1ULL << ST7789V_PIN_RST) |
                           (1ULL << ST7789V_PIN_BL);
    io_conf.mode = GPIO_MODE_OUTPUT;
    gpio_config(&io_conf);

    gpio_set_level(ST7789V_PIN_DC, 0);
    gpio_set_level(ST7789V_PIN_RST, 1);
    gpio_set_level(ST7789V_PIN_BL, 0);

    spi_bus_config_t buscfg = {0};
    buscfg.mosi_io_num = ST7789V_PIN_MOSI;
    buscfg.miso_io_num = ST7789V_PIN_MISO;
    buscfg.sclk_io_num = ST7789V_PIN_SCLK;
    buscfg.quadwp_io_num = -1;
    buscfg.quadhd_io_num = -1;
    buscfg.max_transfer_sz = 4096;

    ESP_ERROR_CHECK(spi_bus_initialize(ST7789V_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO));
    ESP_LOGI(TAG, "SPI bus initialized");

    spi_device_interface_config_t devcfg = {0};
    devcfg.clock_speed_hz = ST7789V_SPI_FREQ;
    devcfg.mode = ST7789V_SPI_MODE;
    devcfg.spics_io_num = ST7789V_PIN_CS;
    devcfg.queue_size = 7;
    devcfg.flags = SPI_DEVICE_HALFDUPLEX;

    ESP_ERROR_CHECK(spi_bus_add_device(ST7789V_SPI_HOST, &devcfg, &spi));
    ESP_LOGI(TAG, "SPI device added, clock=%dHz", ST7789V_SPI_FREQ);

    ESP_LOGI(TAG, "Hardware reset...");
    gpio_set_level(ST7789V_PIN_RST, 0);
    st7789v_delay_ms(10);
    gpio_set_level(ST7789V_PIN_RST, 1);
    st7789v_delay_ms(120);

    ESP_LOGI(TAG, "Sleep out (0x11)...");
    st7789v_write_cmd(0x11);
    st7789v_delay_ms(120);

    ESP_LOGI(TAG, "Pixel format RGB565 (0x3A)...");
    st7789v_write_cmd(0x3A);
    st7789v_write_data_byte(0x55);

    ESP_LOGI(TAG, "MADCTL (0x36)=0xA0 (landscape)...");
    st7789v_write_cmd(0x36);
    st7789v_write_data_byte(0xA0);

    ESP_LOGI(TAG, "Porch (0xB2)...");
    st7789v_write_cmd(0xB2);
    uint8_t porch[] = {0x0C, 0x0C, 0x00, 0x33, 0x33};
    st7789v_write_data(porch, 5);

    ESP_LOGI(TAG, "Gate (0xB7)=0x35...");
    st7789v_write_cmd(0xB7);
    st7789v_write_data_byte(0x35);

    ESP_LOGI(TAG, "VCOM (0xBB)=0x3F...");
    st7789v_write_cmd(0xBB);
    st7789v_write_data_byte(0x3F);

    ESP_LOGI(TAG, "LCM (0xC0)=0x2C...");
    st7789v_write_cmd(0xC0);
    st7789v_write_data_byte(0x2C);

    ESP_LOGI(TAG, "Power1 (0xC2)=0x01...");
    st7789v_write_cmd(0xC2);
    st7789v_write_data_byte(0x01);

    ESP_LOGI(TAG, "Power2 (0xC3)=0x11...");
    st7789v_write_cmd(0xC3);
    st7789v_write_data_byte(0x11);

    ESP_LOGI(TAG, "Power3 (0xC4)=0x20...");
    st7789v_write_cmd(0xC4);
    st7789v_write_data_byte(0x20);

    ESP_LOGI(TAG, "Frame rate (0xC6)=0x0F...");
    st7789v_write_cmd(0xC6);
    st7789v_write_data_byte(0x0F);

    ESP_LOGI(TAG, "Gamma+ (0xE0)...");
    st7789v_write_cmd(0xE0);
    uint8_t gamma_p[] = {0xD0, 0x08, 0x11, 0x08, 0x0C, 0x15, 0x39, 0x33,
                         0x50, 0x36, 0x13, 0x14, 0x29, 0x2D};
    st7789v_write_data(gamma_p, 14);

    ESP_LOGI(TAG, "Gamma- (0xE1)...");
    st7789v_write_cmd(0xE1);
    uint8_t gamma_n[] = {0xD0, 0x08, 0x10, 0x08, 0x06, 0x06, 0x39, 0x44,
                         0x51, 0x0B, 0x16, 0x14, 0x2F, 0x31};
    st7789v_write_data(gamma_n, 14);

    ESP_LOGI(TAG, "Display ON (0x29)...");
    st7789v_write_cmd(0x29);
    st7789v_delay_ms(100);

    ESP_LOGI(TAG, "Clearing screen to black...");
    st7789v_fill(0x0000);

    ESP_LOGI(TAG, "Backlight ON");
    st7789v_set_backlight(1);
    st7789v_delay_ms(30);

    is_init = true;
    ESP_LOGI(TAG, "ST7789V init complete: %dx%d", ST7789V_WIDTH, ST7789V_HEIGHT);
}

void st7789v_deinit(void)
{
    if (!is_init) return;
    st7789v_set_backlight(0);
    spi_bus_remove_device(spi);
    spi_bus_free(ST7789V_SPI_HOST);
    is_init = false;
}

void st7789v_set_backlight(uint8_t on)
{
    gpio_set_level(ST7789V_PIN_BL, on ? 1 : 0);
}

void st7789v_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    uint8_t data[4];

    st7789v_write_cmd(0x2A);
    data[0] = (x0 >> 8) & 0xFF;
    data[1] = x0 & 0xFF;
    data[2] = (x1 >> 8) & 0xFF;
    data[3] = x1 & 0xFF;
    st7789v_write_data(data, 4);

    st7789v_write_cmd(0x2B);
    data[0] = (y0 >> 8) & 0xFF;
    data[1] = y0 & 0xFF;
    data[2] = (y1 >> 8) & 0xFF;
    data[3] = y1 & 0xFF;
    st7789v_write_data(data, 4);

    st7789v_write_cmd(0x2C);
}

void st7789v_write_pixels(const uint16_t *buf, uint32_t len)
{
    const uint32_t chunk = 240;
    static uint8_t tx_buf[480];
    gpio_set_level(ST7789V_PIN_DC, 1);

    for (uint32_t off = 0; off < len; off += chunk) {
        uint32_t n = len - off;
        if (n > chunk) n = chunk;

        for (uint32_t i = 0; i < n; i++) {
            uint16_t c = buf[off + i];
            tx_buf[i * 2]     = (c >> 8) & 0xFF;
            tx_buf[i * 2 + 1] = c & 0xFF;
        }

        spi_transaction_t t = {0};
        t.length = n * 16;
        t.tx_buffer = tx_buf;
        spi_device_polling_transmit(spi, &t);
    }
}

void st7789v_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const uint16_t *buf)
{
    uint32_t w = x2 - x1 + 1;
    uint32_t h = y2 - y1 + 1;
    uint32_t len = w * h;

    st7789v_set_window((uint16_t)x1, (uint16_t)y1, (uint16_t)x2, (uint16_t)y2);
    st7789v_write_pixels(buf, len);
}

void st7789v_fill(uint16_t color)
{
    static uint8_t tx_buf[ST7789V_WIDTH * 2];

    uint8_t hi = (color >> 8) & 0xFF;
    uint8_t lo = color & 0xFF;
    for (uint32_t i = 0; i < ST7789V_WIDTH; i++) {
        tx_buf[i * 2]     = hi;
        tx_buf[i * 2 + 1] = lo;
    }

    st7789v_set_window(0, 0, ST7789V_WIDTH - 1, ST7789V_HEIGHT - 1);
    gpio_set_level(ST7789V_PIN_DC, 1);

    for (uint32_t y = 0; y < ST7789V_HEIGHT; y++) {
        spi_transaction_t t = {0};
        t.length = ST7789V_WIDTH * 16;
        t.tx_buffer = tx_buf;
        spi_device_polling_transmit(spi, &t);
    }
}
