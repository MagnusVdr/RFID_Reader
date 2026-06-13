#include "st25r3912_hal.h"

/**
 * @brief Modifies specific bits in a register (Read-Modify-Write).
 * * @param hw Pointer to hardware context.
 * @param reg_addr Register address.
 * @param mask Bits to change.
 * @param value Value to set into bits.
 */
esp_err_t st25_modify_register(st25r3912_hw_t *hw, uint8_t reg_addr, uint8_t mask, uint8_t value)
{
    uint8_t current_val = 0;
    esp_err_t err = st25_read_register(hw, reg_addr, &current_val);
    if (err != ESP_OK) return err;

    uint8_t new_val = (current_val & ~mask) | (value & mask);

    if (new_val != current_val) {
        err = st25_write_register(hw, reg_addr, new_val);
    }
    
    return err;
}

esp_err_t st25_read_fifo(st25r3912_hw_t *hw, uint8_t *buf, uint8_t len)
{
    // Reading from the FIFO uses a special SPI command: 10bf ffff 
    // where b=0 (read), fffff=0 (FIFO address). Mask = 0x80.
    uint8_t tx_buf[1] = { 0x80 }; // FIFO Read command
    
    spi_transaction_t t = {0};
    t.length = 8; // 1 byte command
    t.rxlength = len * 8; // length in bits to read back
    t.tx_buffer = tx_buf;
    t.rx_buffer = buf;

    return spi_device_polling_transmit(hw->spi_handle, &t);
}

/**
 * @brief Write a single byte to an ST25R3912 register.
 *
 * @param hw Pointer to the ST25R3912 hardware configuration struct.
 * @param reg_addr The 6-bit register address to write to.
 * @param data The 8-bit data payload to write.
 * @return esp_err_t ESP_OK on success, or an ESP-IDF error code on failure.
 */
esp_err_t st25_write_register(st25r3912_hw_t *hw, uint8_t reg_addr, uint8_t data)
{
    // Safety check for null pointers
    if (hw == NULL || hw->spi_handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    spi_transaction_t t = {0};

    t.length = 16; // 8 bits command/address + 8 bits data
    t.flags = SPI_TRANS_USE_TXDATA; // Use the internal tx_data array for speed
    
    // Byte 0: SPI Operation Mode bits (00 for write) + 6-bit address
    t.tx_data[0] = ST25_SPI_WRITE_MASK | (reg_addr & 0x3F);
    
    // Byte 1: The data byte to write
    t.tx_data[1] = data;

    // Execute a blocking SPI transfer
    return spi_device_polling_transmit(hw->spi_handle, &t);
}

/**
 * @brief Read a single byte from an ST25R3912 register.
 *
 * @param hw Pointer to the ST25R3912 hardware configuration struct.
 * @param reg_addr The 6-bit register address to read from.
 * @param data Pointer to a uint8_t where the read data will be stored.
 * @return esp_err_t ESP_OK on success, or an ESP-IDF error code on failure.
 */
esp_err_t st25_read_register(st25r3912_hw_t *hw, uint8_t reg_addr, uint8_t *data)
{
    if (hw == NULL || hw->spi_handle == NULL || data == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    // Byte 0: Command, Byte 1: Dummy data to clock MISO
    uint8_t tx_buf[2] = {
        ST25_SPI_READ_MASK | (reg_addr & 0x3F), 
        0x00 
    };
    uint8_t rx_buf[2] = {0};

    spi_transaction_t t = {0};
    t.length = 16; 
    t.tx_buffer = tx_buf;
    t.rx_buffer = rx_buf;

    esp_err_t err = spi_device_polling_transmit(hw->spi_handle, &t);
    
    if (err == ESP_OK) {
        // rx_buf[0] contains what the chip sent during tx_buf[0] (High-Z)
        // rx_buf[1] contains the actual register data sent during the dummy byte
        *data = rx_buf[1]; 
    }
    
    return err;
}

/**
 * @brief Sends a direct command to the ST25R3912.
 */
esp_err_t st25_execute_command(st25r3912_hw_t *hw, st25_direct_command_t cmd)
{
    if (hw == NULL || hw->spi_handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    // Direct command is 1 byte: SPI Mode bits (11) + 6-bit command code
    uint8_t tx_buf[1] = { ST25_SPI_CMD_MASK | (cmd & 0x3F) };

    spi_transaction_t t = {0};
    t.length = 8; // Only 8 bits required for a direct command
    t.tx_buffer = tx_buf;

    return spi_device_polling_transmit(hw->spi_handle, &t);
}