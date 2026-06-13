#ifndef ST25R3912_H
#define ST25R3912_H

#include <stdint.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_timer.h"

#define OP_CTRL_FIELD_ON  0xC8 // Oscillator, Receiver, and Transmitter ON
#define OP_CTRL_FIELD_OFF 0xC0 // Oscillator, Receiver ON. Transmitter OFF

#define ST25R3912_REG_FIFO_RX_STATUS_1 0x1A
#define ST25R3912_REG_FIFO_RX_STATUS_2 0x1B
#define ST25R3912_REG_MAIN_INT         0x17

// Define our golden ratio for 424 kHz Stream Mode
#define BITS_PER_HALF_BIT 8  // 18.88 us / 2.36 us = 8 bits

/**
 * @brief Tx Driver Resistance Levels.
 * Lower resistance = Higher output power & current draw.
 * Higher resistance = Lower output power & current draw.
 */
typedef enum {
    ST25_TX_RES_MIN_POWER_MAX = 0x00, // 0: Minimum resistance (~1.5 Ohm), Maximum Power
    ST25_TX_RES_1             = 0x01,
    ST25_TX_RES_2             = 0x02,
    ST25_TX_RES_3             = 0x03,
    ST25_TX_RES_4             = 0x04,
    ST25_TX_RES_5             = 0x05,
    ST25_TX_RES_6             = 0x06,
    ST25_TX_RES_7             = 0x07,
    ST25_TX_RES_8             = 0x08,
    ST25_TX_RES_9             = 0x09,
    ST25_TX_RES_10            = 0x0A,
    ST25_TX_RES_11            = 0x0B,
    ST25_TX_RES_12            = 0x0C,
    ST25_TX_RES_13            = 0x0D,
    ST25_TX_RES_14            = 0x0E,
    ST25_TX_RES_MAX_POWER_MIN = 0x0F  // 15: Maximum resistance, Minimum Power
} st25_tx_res_t;

// Status of a single slot read
typedef enum {
    SLOT_NO_RESPONSE = 0,
    SLOT_VALID_TAG   = 1,
    SLOT_COLLISION   = 2
} slot_status_t;

// A mask used to isolate colliding tags
typedef struct {
    uint8_t length;     // Number of bits in the mask
    uint32_t val_lower; // Lower 32 bits of the mask (Bits 0-31)
    uint32_t val_upper; // Upper 32 bits of the mask (Bits 32-63)
} iso15693_mask_t;

// The Anticollision Stack (Max depth is 16 for a 64-bit UID)
typedef struct {
    iso15693_mask_t items[16];
    int top;
} anticollision_stack_t;


esp_err_t st25r3912_init(st25r3912_t *hnd);
esp_err_t st25_setup(st25r3912_hw_t *hw);
esp_err_t st25_config_rx_for_iso15693(st25r3912_hw_t *hw);
esp_err_t st25_config_stream_mode_iso15693(st25r3912_hw_t *hw);
void st25_run_anticollision(st25r3912_hw_t *hw);
esp_err_t st25_send_inventory_cmd(st25r3912_hw_t *hw, iso15693_mask_t mask);
esp_err_t st25_set_tx_driver_resistance(st25r3912_hw_t *hw, st25_tx_res_t unmodulated_res, st25_tx_res_t modulated_res);
esp_err_t send_16_slot_inventory(st25r3912_hw_t *hw);
slot_status_t st25_listen_to_slot(st25r3912_hw_t *hw, uint8_t *decoded_uid);
esp_err_t st25_field_on(st25r3912_hw_t *hw);
esp_err_t st25_field_off(st25r3912_hw_t *hw);
esp_err_t st25_send_iso15693_sof(st25r3912_hw_t *hw);
esp_err_t st25_send_iso15693_eof(st25r3912_hw_t *hw);
esp_err_t st25_send_iso15693_data(st25r3912_hw_t *hw, const uint8_t *data, size_t len);
void stack_push(anticollision_stack_t *stack, iso15693_mask_t mask);
iso15693_mask_t stack_pop(anticollision_stack_t *stack);


uint16_t st25_calculate_crc16(const uint8_t *data, size_t len) ;

esp_err_t st25_test_anticollision(st25r3912_hw_t *hw);

esp_err_t st25_enable_mcu_clk_output(st25r3912_hw_t *hw, bool enable);

#endif // ST25R3912_H
