#ifndef ST25R3912_HAL_H
#define ST25R3912_HAL_H

#include "st25r3912_registers.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

// SPI Mode Masks (Bits 6 and 7 define the operation)
#define ST25_SPI_WRITE_MASK  (0x00U) // 00xx xxxx
#define ST25_SPI_READ_MASK   (0x40U) // 01xx xxxx
#define ST25_SPI_FIFO_MASK   (0x80U) // 10xx xxxx (FIFO load/read)
#define ST25_SPI_CMD_MASK    (0xC0U) // 11xx xxxx (Direct Command)

// Direct Commands
typedef enum {
    ST25_CMD_SET_DEFAULT              = 0x01, // C1 (0x01 | 0xC0)
    ST25_CMD_CLEAR                    = 0x02, // C2
    ST25_CMD_TRANSMIT_WITH_CRC        = 0x04, // C4
    ST25_CMD_TRANSMIT_WITHOUT_CRC     = 0x05, // C5
    ST25_CMD_TRANSMIT_REQA            = 0x06, // C6
    ST25_CMD_TRANSMIT_WUPA            = 0x07, // C7
    ST25_CMD_NFC_INITIAL_FIELD_ON     = 0x08, // C8
    ST25_CMD_NFC_RESPONSE_FIELD_ON    = 0x09, // C9
    ST25_CMD_NFC_RESPONSE_FIELD_ON_N0 = 0x0A, // CA
    ST25_CMD_GO_TO_NORMAL_NFC_MODE    = 0x0B, // CB
    ST25_CMD_ANALOG_PRESET            = 0x0C, // CC
    ST25_CMD_MASK_RECEIVE_DATA        = 0x10, // D0
    ST25_CMD_UNMASK_RECEIVE_DATA      = 0x11, // D1
    ST25_CMD_MEASURE_AMPLITUDE        = 0x13, // D3
    ST25_CMD_SQUELCH                  = 0x14, // D4
    ST25_CMD_RESET_RX_GAIN            = 0x15, // D5
    ST25_CMD_ADJUST_REGULATORS        = 0x16, // D6
    ST25_CMD_CALIBRATE_MOD_DEPTH      = 0x17, // D7
    ST25_CMD_MEASURE_PHASE            = 0x19, // D9
    ST25_CMD_CLEAR_RSSI               = 0x1A, // DA
    ST25_CMD_TRANSPARENT_MODE         = 0x1C, // DC
    ST25_CMD_MEASURE_POWER_SUPPLY     = 0x1F, // DF
    ST25_CMD_START_GP_TIMER           = 0x20, // E0
    ST25_CMD_START_WAKEUP_TIMER       = 0x21, // E1
    ST25_CMD_START_MASK_RX_TIMER      = 0x22, // E2
    ST25_CMD_START_NO_RESPONSE_TIMER  = 0x23, // E3
    ST25_CMD_TEST_ACCESS              = 0x3C  // FC
} st25_direct_command_t;

/*
 * Hardware configuration mapping
 */
typedef struct {
    spi_host_device_t   spi_host;   // e.g., SPI2_HOST (FSPI) or SPI3_HOST (HSPI)
    spi_device_handle_t spi_handle; // Handle assigned by spi_bus_add_device()
    gpio_num_t          cs_pin;     // Chip Select pin
    gpio_num_t          irq_pin;    // Hardware Interrupt pin
} st25r3912_hw_t;

typedef struct 
{
    st25r3912_hw_t      hw;         // Hardware-specific SPI/GPIO mapping
    st25r3912_registers registers;
}st25r3912_t;

esp_err_t st25_modify_register(st25r3912_hw_t *hw, uint8_t reg_addr, uint8_t mask, uint8_t value);
esp_err_t st25_read_fifo(st25r3912_hw_t *hw, uint8_t *buf, uint8_t len);
esp_err_t st25_read_register(st25r3912_hw_t *hw, uint8_t reg_addr, uint8_t *data);
esp_err_t st25_write_register(st25r3912_hw_t *hw, uint8_t reg_addr, uint8_t data);
esp_err_t st25_execute_command(st25r3912_hw_t *hw, st25_direct_command_t cmd);

#endif // ST25R3912_HAL_H
