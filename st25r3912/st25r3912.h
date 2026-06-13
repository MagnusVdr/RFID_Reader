#ifndef ST25R3912_H
#define ST25R3912_H

#include <stdint.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "st25r3912_hal.h"


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

esp_err_t st25_setup(st25r3912_hw_t *hw);
esp_err_t st25_set_tx_driver_resistance(st25r3912_hw_t *hw, st25_tx_res_t unmodulated_res, st25_tx_res_t modulated_res);

#endif // ST25R3912_H
