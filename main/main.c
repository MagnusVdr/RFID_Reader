#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "st25r3912.h"

// Define the ESP32-S2 SPI pins requested
#define PIN_NUM_CS   5
#define PIN_NUM_CLK  6
#define PIN_NUM_MOSI 7
#define PIN_NUM_MISO 8
#define PIN_NUM_IRQ  9

#define LED_PIN      33

static const char *TAG = "MAIN";

st25r3912_t st25 = {0};

void app_main(void)
{
    // 1. Initialize LED Pin for a heartbeat indicator
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    // 2. Configure the physical SPI bus
    spi_bus_config_t buscfg = {
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1, // Not used
        .quadhd_io_num = -1, // Not used
        .max_transfer_sz = 32 // We only do small 16-bit transfers, 32 bytes is plenty
    };

    // Initialize the SPI bus on SPI2_HOST (FSPI on the ESP32-S2)
    esp_err_t ret = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize SPI bus.");
        return;
    }

    // 3. Populate the ST25 hardware context
    st25.hw.spi_host = SPI2_HOST;
    st25.hw.cs_pin = PIN_NUM_CS;
    st25.hw.irq_pin = PIN_NUM_IRQ;

    // 4. Initialize the ST25R3912 and test the connection
    ret = st25r3912_init(&st25);
    if (ret == ESP_OK) 
    {
        // Read the bottom 3 bits of the cached identity register for the revision
        uint8_t revision = (st25.registers.ic_identity.byte & 0x07);
        ESP_LOGI(TAG, "ST25R3912 successfully initialized! Silicon Revision: %d", revision);
    } else {
        ESP_LOGE(TAG, "Failed to initialize ST25R3912. Error: %s", esp_err_to_name(ret));
        // TWEAK 1: Trap the processor here if the chip is dead/unplugged
        while (1) { vTaskDelay(pdMS_TO_TICKS(1000)); }
    }

    st25_setup(&st25.hw);

    st25_set_tx_driver_resistance(&st25.hw, 0, 15);

    while (1) {
        // Toggle LED
        static int led_state = 0;
        // gpio_set_level(LED_PIN, led_state);
        led_state = !led_state;

        vTaskDelay(pdMS_TO_TICKS(1000)); 
    }
}