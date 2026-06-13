#include "st25r3912.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG_ST25 = "ST25_DRV";

/**
 * @brief Executes the official power-up sequence and calibrates the AFE.
 */
esp_err_t st25_setup(st25r3912_hw_t *hw) 
{
    if (hw == NULL || hw->spi_handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err;
    ESP_LOGI(TAG_ST25, "Starting power-up sequence...");

    err = st25_modify_register(hw, ST25R3912_REG_IO_CONF_1, 0x08, 0x08);
    if (err != ESP_OK) return err;

    // Tell the chip we have a 5V supply (Clear 'sup3V' bit 7 in IO_CONF_2)
    err = st25_modify_register(hw, ST25R3912_REG_IO_CONF_2, 0x80, 0x00);
    if (err != ESP_OK) return err;

    // 1. Enable the oscillator and regulators (Set 'en' bit 7 in Op Control 0x02)
    err = st25_modify_register(hw, ST25R3912_REG_OP_CONTROL, 0x80, 0x80);
    if (err != ESP_OK) return err;

    // 2. Poll the Auxiliary Display Register (0x30) until 'osc_ok' (bit 4) is high
    uint8_t aux_disp = 0;
    int timeout = 100; // 100 ms timeout
    do {
        err = st25_read_register(hw, ST25R3912_REG_AUX_DISPLAY, &aux_disp);
        if (err != ESP_OK) return err;
        
        if (aux_disp & 0x10) {
            break; // Oscillator is stable!
        }
        vTaskDelay(pdMS_TO_TICKS(1));
        timeout--;
    } while (timeout > 0);

    if (timeout == 0) {
        ESP_LOGE(TAG_ST25, "Timeout waiting for oscillator to stabilize!");
        return ESP_ERR_TIMEOUT;
    }

    // 3. Adjust Regulators for optimal PSRR based on our 5V supply setting
    err = st25_execute_command(hw, ST25_CMD_ADJUST_REGULATORS);
    if (err != ESP_OK) return err;
    vTaskDelay(pdMS_TO_TICKS(5)); 

    // 4. Set Target AM Modulation Depth to ~10% (0x0E maps to 9.9%)
    err = st25_write_register(hw, ST25R3912_REG_AM_MOD_DEPTH_CONTROL, 0x0E);
    if (err != ESP_OK) return err;

    // 5. Run the AM Modulation Calibration
    err = st25_execute_command(hw, ST25_CMD_CALIBRATE_MOD_DEPTH);
    if (err != ESP_OK) return err;
    vTaskDelay(pdMS_TO_TICKS(2)); 

    ESP_LOGI(TAG_ST25, "ST25R3912 setup and calibrated successfully for 27MHz/5V.");

    return ESP_OK;
}

/**
 * @brief Sets the Tx driver output resistance (impedance).
 * * @param hw Pointer to the hardware context.
 * @param unmodulated_res Resistance index for unmodulated carrier (0 to 15).
 * 0 = minimum resistance (max power). 15 = maximum resistance (min power).
 * @param modulated_res Resistance index during AM modulation (0 to 15).
 * @return esp_err_t ESP_OK on success.
 */
esp_err_t st25_set_tx_driver_resistance(st25r3912_hw_t *hw, st25_tx_res_t unmodulated_res, st25_tx_res_t modulated_res)
{
    // The bitwise & 0x0F naturally enforces the 4-bit limit without needing an IF statement
    uint8_t tx_driver_val = ((modulated_res & 0x0F) << 4) | (unmodulated_res & 0x0F);
    return st25_write_register(hw, ST25R3912_REG_TX_DRIVER, tx_driver_val);
}
