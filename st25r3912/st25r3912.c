#include "st25r3912.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define OP_CTRL_TX_ON  0x88  // en(1) + rx_en(0) + tx_en(1) 
#define OP_CTRL_TX_OFF 0x80  // en(1) + rx_en(0) + tx_en(0)
#define OP_CTRL_RX_ON  0xD8  // en(1) + rx_en(1) + rx_man(1) + tx_en(1) <-- FORCED ON!



static const char *TAG_ST25 = "ST25_DRV";
static const char *TAG_ALOHA = "ALOHA";

static bool st25_find_sof(const uint8_t *data, size_t max_bytes, size_t *sync_bit_index);
static inline uint32_t get_fifo_bit(const uint8_t *data, size_t max_bytes, size_t bit_index);
static uint32_t get_half_bit_state(const uint8_t *data, size_t max_bytes, size_t start_bit_index);

/**
 * @brief Initializes the ST25R3912 SPI device and verifies communication.
 *
 * @param hnd Pointer to the driver context. The hnd->hw struct must have 
 * spi_host and cs_pin populated before calling this.
 * @return esp_err_t ESP_OK on success, ESP_ERR_NOT_FOUND if chip ID is wrong.
 */
esp_err_t st25r3912_init(st25r3912_t *hnd) 
{
    if (hnd == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = ESP_OK;

    // 1. Configure the SPI Device Interface
    spi_device_interface_config_t dev_cfg = {
        .clock_speed_hz = 4 * 1000 * 1000,      // 4 MHz (Datasheet max is 6 MHz )
        .mode = 1,                              // CPOL = 0, CPHA = 1 (Sample on falling edge )
        .spics_io_num = hnd->hw.cs_pin,         // ESP-IDF handles the CS pin automatically
        .queue_size = 7,                        // Number of queued transactions
    };

    // 2. Attach the ST25R3912 to the SPI bus
    ret = spi_bus_add_device(hnd->hw.spi_host, &dev_cfg, &hnd->hw.spi_handle);
    if (ret != ESP_OK) {
        return ret;
    }

    // 3. Test Communication by reading the IC Identity Register
    ret = st25_read_register(&hnd->hw, ST25R3912_REG_IC_IDENTITY, &hnd->registers.ic_identity.byte);
    if (ret != ESP_OK) {
        return ret;
    }

    // 4. Verify the Chip ID
    // The top 5 bits (ic_type) for the ST25R3912 are 0b00001.
    // 0b00001000 in hex is 0x08.
    if ((hnd->registers.ic_identity.byte & 0xF8) != 0x08) {
        return ESP_ERR_NOT_FOUND;
    }

    return ret;
}

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

    // 0. HARDWARE ENVIRONMENT SETUP (Must be done before enabling oscillator)
    
    // Tell the chip we have a 27.12 MHz crystal (Set 'osc' bit 3 in IO_CONF_1)
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

esp_err_t st25_config_rx_for_iso15693(st25r3912_hw_t *hw) 
{
    esp_err_t err;

    // 1. RX Configuration Register 1 (0x0A)
    // - ch1_rx_en = 1 (Enable AM demodulator)
    // - ch2_rx_en = 1 (Enable PM demodulator)
    // Mask: 0xC0 (bits 7 and 6). Value: 0xC0
    err = st25_modify_register(hw, ST25R3912_REG_RX_CONF_1, 0xC0, 0xC0);
    if (err != ESP_OK) return err;

    // 2. RX Configuration Register 2 (0x0B) - The Filters 
    // - rx_lp (Bits 6:4): Low Pass Filter. 0x01 = 800 kHz cutoff.
    // - rx_hp (Bits 2:0): High Pass Filter. 0x02 = 200 kHz cutoff.
    // This creates a perfect bandpass window for our 424 kHz subcarrier!
    // Mask: 0x77 (0111 0111). Value: 0x12 (0001 0010)
    err = st25_modify_register(hw, ST25R3912_REG_RX_CONF_2, 0x77, 0x12);
    if (err != ESP_OK) return err;

    // 3. RX Configuration Register 3 (0x0C) - Gain Settings
    // Default AGC (Automatic Gain Control) is usually fine, but we ensure 
    // it is enabled and set to a standard starting gain for vicinity tags.
    // - agc_en = 1 (Bit 7)
    // - rg_nfc = 0 (Bit 6, we are not doing NFC IP-1)
    err = st25_modify_register(hw, ST25R3912_REG_RX_CONF_3, 0xC0, 0x80);
    if (err != ESP_OK) return err;

    // 4. RX Configuration Register 4 (0x0D) - AGC Action
    // - agc_m = 1 (Bit 7, operate on both AM and PM channels)
    err = st25_modify_register(hw, ST25R3912_REG_RX_CONF_4, 0x80, 0x80);
    
    return err;
}

/**
 * @brief Configures the ST25R3912 for ISO 15693 Sub-carrier Stream Mode.
 * * @param hw Pointer to the hardware context.
 * @return esp_err_t ESP_OK on success.
 */
esp_err_t st25_config_stream_mode_iso15693(st25r3912_hw_t *hw) 
{
    esp_err_t err;

    // 1. Mode Definition Register (0x03)
    // We need to set the Initiator Operation Mode (om) bits [7:4] to 1110 (Stream Mode).
    // Mask: 0xF0 (1111 0000). Value: 0xE0 (1110 0000).
    err = st25_modify_register(hw, ST25R3912_REG_MODE_DEF, 0xF0, 0xE0);
    if (err != ESP_OK) return err;

    // 2. Stream Mode Definition Register (0x08)
    // - scf[1:0] (Bits 7:6): Sub-carrier frequency. 01 = fc/32 (424 kHz).
    // - scp[1:0] (Bits 5:4): Sub-carrier pulses. 00 = 1 pulse (report every pulse).
    // - stx[2:0] (Bits 2:0): Tx Type. 000 = OOK (100% AM)
    // Mask: 0xFF. Value: 0x40 (0100 0000)
    err = st25_modify_register(hw, ST25R3912_REG_STREAM_MODE_DEF, 0xFF, 0x40);
    
    return err;
}

/**
 * @brief Executes the full ISO 15693 Anticollision algorithm.
 */
void st25_run_anticollision(st25r3912_hw_t *hw)
{
    anticollision_stack_t stack = { .top = 0 };

    // 1. Start with a completely empty mask (Length = 0)
    iso15693_mask_t initial_mask = {0};
    stack_push(&stack, initial_mask);

    // 2. Loop until we have resolved every collision on the stack
    while (stack.top > 0) {
        
        iso15693_mask_t current_mask = stack_pop(&stack);
        
        ESP_LOGI(TAG_ALOHA, "Starting Inventory. Mask Len: %d", current_mask.length);

        st25_send_inventory_cmd(hw, current_mask);

        // Process all 16 slots
        for (uint8_t slot = 0; slot < 16; slot++) {
            
            if (slot > 0) {
                st25_send_iso15693_eof(hw);
            }

            // LISTEN TO THE SLOT (Now passing an 8-byte array)
            uint8_t tag_uid[8] = {0};
            slot_status_t status = st25_listen_to_slot(hw, tag_uid);

            if (status == SLOT_VALID_TAG) {
                // WE FOUND A TAG! Print the 8 bytes.
                ESP_LOGI(TAG_ALOHA, "Found Tag in Slot %d! UID: %02X%02X%02X%02X%02X%02X%02X%02X", 
                         slot, tag_uid[7], tag_uid[6], tag_uid[5], tag_uid[4], 
                         tag_uid[3], tag_uid[2], tag_uid[1], tag_uid[0]);
            } 
            else if (status == SLOT_COLLISION) {
                ESP_LOGW(TAG_ALOHA, "Collision detected in Slot %d. Pushing to stack.", slot);

                // NATIVE 32-BIT COLLISION RESOLUTION
                iso15693_mask_t new_mask = current_mask;
                new_mask.length = current_mask.length + 4;
                
                if (current_mask.length < 32) {
                    // Still building the lower 32 bits
                    new_mask.val_lower |= ((uint32_t)slot << current_mask.length);
                } else {
                    // Crossed the boundary, shift into the upper 32 bits
                    new_mask.val_upper |= ((uint32_t)slot << (current_mask.length - 32));
                }

                stack_push(&stack, new_mask);
            }
        }
    }
    
    // ESP_LOGI(TAG_ALOHA, "Anticollision complete! All tags identified.");
}



/**
 * @brief Builds and transmits an ISO 15693 Inventory Command with a variable mask.
 * * @param hw Pointer to the hardware context.
 * @param mask The mask struct containing the length and value to append.
 * @return esp_err_t ESP_OK on success.
 */
esp_err_t st25_send_inventory_cmd(st25r3912_hw_t *hw, iso15693_mask_t mask) 
{
    // Max payload size: Flags(1) + Cmd(1) + MaskLen(1) + MaskValue(up to 8) + CRC(2) = 13 bytes
    uint8_t payload[13] = {0}; 
    size_t payload_len = 0;

    // 1. Flags Byte
    // 0x06 = 0000 0110b
    // Bit 0: 0 (Single sub-carrier)
    // Bit 1: 1 (High data rate)
    // Bit 2: 1 (Inventory command)
    // Bit 3: 0 (No Protocol Extension)
    // Bit 4: 0 (No AFI)
    // Bit 5: 0 (16 Slots)
    payload[payload_len] = 0x06;
    payload_len++;

    // 2. Command Code
    // 0x01 is the standard Inventory command code
    payload[payload_len] = 0x01;
    payload_len++;

    // 3. Mask Length
    // How many bits of the mask are valid
    payload[payload_len] = mask.length;
    payload_len++;

    // 4. Mask Value (Dynamic Length)
    if (mask.length > 0) {
        // Calculate how many bytes are needed to hold 'mask.length' bits
        uint8_t mask_bytes = (mask.length + 7) / 8;
        
        for (int i = 0; i < mask_bytes; i++) {
            if (i < 4) {
                // Bytes 0-3 come from the lower 32 bits
                payload[payload_len++] = (uint8_t)((mask.val_lower >> (i * 8)) & 0xFF);
            } else {
                // Bytes 4-7 come from the upper 32 bits
                payload[payload_len++] = (uint8_t)((mask.val_upper >> ((i - 4) * 8)) & 0xFF);
            }
        }
    }

    // 5. Calculate and append the CRC-16
    // The CRC is calculated over the Flags, Command, Mask Length, and Mask Value
    uint16_t crc = st25_calculate_crc16(payload, payload_len);
    
    // ISO 15693 transmits CRC Least Significant Byte (LSB) first
    payload[payload_len++] = (uint8_t)(crc & 0xFF);
    payload[payload_len++] = (uint8_t)((crc >> 8) & 0xFF);

    // 6. Transmit the complete packet using our primitive functions
    esp_err_t err;
    
    err = st25_send_iso15693_sof(hw);
    if (err != ESP_OK) return err;
    
    err = st25_send_iso15693_data(hw, payload, payload_len);
    if (err != ESP_OK) return err;
    
    // Important: For the Inventory Command, we send an EOF immediately after the data 
    // to mark the end of the request and the beginning of Slot 0.
    err = st25_send_iso15693_eof(hw);
    
    return err;
}

/**
 * @brief Listens to an ISO 15693 slot, decodes the Manchester stream, and detects collisions.
 * * @param hw Pointer to hardware context.
 * @param decoded_uid Pointer to store the successfully decoded 64-bit UID.
 * @return slot_status_t SLOT_VALID_TAG, SLOT_COLLISION, or SLOT_NO_RESPONSE.
 */
slot_status_t st25_listen_to_slot(st25r3912_hw_t *hw, uint8_t *decoded_uid)
{
    // 1. Wait for data to arrive in the FIFO (Wait for RX interrupt)
    uint64_t start_time = esp_timer_get_time();
    uint8_t irq_status = 0;
    bool data_received = false;

    // ISO 15693 tags must reply within ~320 us (t1 time). We give a 2ms timeout.
    while ((esp_timer_get_time() - start_time) < 2000) {
        st25_read_register(hw, ST25R3912_REG_MAIN_INT, &irq_status);
        
        if (irq_status != 0) {
            
            // If an Error Interrupt is flagged, we MUST read Register 0x19 to clear it!
            if (irq_status & 0x02) {
                uint8_t err_status = 0;
                st25_read_register(hw, 0x19, &err_status);
                ESP_LOGW("RX_DBG", "Error IRQ Cleared! Reg 0x19: 0x%02X", err_status);
            }
            
            // 0x40 is the FIFO Water Level (wl) interrupt. 
            // It means the tag is talking and the FIFO is filling up!
            if (irq_status & 0x40) { 
                // Delay just a tiny bit to let the tag finish its 3ms reply
                esp_rom_delay_us(1000); 
                data_received = true;
                break;
            }
        }
    }

    if (!data_received) {
        ESP_LOGW("RX_DBG", "Timeout: No RX Interrupt");
        return SLOT_NO_RESPONSE; // Nobody talked in this slot
    }

    // 2. Read how many bytes are in the FIFO
    uint8_t fifo_bytes_1 = 0, fifo_bytes_2 = 0;
    st25_read_register(hw, ST25R3912_REG_FIFO_RX_STATUS_1, &fifo_bytes_1);
    st25_read_register(hw, ST25R3912_REG_FIFO_RX_STATUS_2, &fifo_bytes_2);
    
    // Combine lengths (ST25R3912 has a 96-byte FIFO)
    uint8_t num_bytes = fifo_bytes_1 | ((fifo_bytes_2 & 0x01) << 8);

    if (num_bytes == 0) {
        return SLOT_NO_RESPONSE;
    }

    // 3. Pull the raw Stream Mode bitstream from the FIFO
    uint8_t raw_fifo_data[96] = {0};
    st25_read_fifo(hw, raw_fifo_data, num_bytes);

    // 4. The Software Manchester Decoder State Machine
    // -------------------------------------------------------------------------
    
    size_t sync_bit_index = 0;
    
    // Step 4a: Slide the window to find the exact Start of Frame (SOF)
    if (!st25_find_sof(raw_fifo_data, num_bytes, &sync_bit_index)) 
    {
        ESP_LOGW("RX_DBG", "RX IRQ fired, but no valid SOF found in %d bytes", num_bytes);
        return SLOT_NO_RESPONSE; // No valid SOF found, just noise
    }

    // Step 4b: Decode the payload
    // A standard ISO 15693 Inventory response contains 12 bytes (96 bits):
    // Flags (1) + DSFID (1) + UID (8) + CRC (2)
    uint8_t decoded_bytes[12] = {0};
    int decoded_bits = 0;
    size_t current_bit_idx = sync_bit_index;
    size_t total_fifo_bits = num_bytes * 8;

    while (decoded_bits < 96) {
        // Ensure we have enough FIFO data left for a full 16-bit Manchester window
        if (current_bit_idx + (2 * BITS_PER_HALF_BIT) > total_fifo_bits) {
            break; 
        }

        // Sample the two halves of the Manchester bit
        int hb1 = get_half_bit_state(raw_fifo_data, num_bytes, current_bit_idx);
        int hb2 = get_half_bit_state(raw_fifo_data, num_bytes, current_bit_idx + BITS_PER_HALF_BIT);

        if (hb1 == -1 || hb2 == -1) break; // Safety bounds check

        int decoded_logic_bit = 0;

        // Evaluate the Manchester transitions
        if (hb1 == 0 && hb2 == 1) {
            decoded_logic_bit = 0; // Logic 0
        } 
        else if (hb1 == 1 && hb2 == 0) {
            decoded_logic_bit = 1; // Logic 1
        } 
        else if (hb1 == 1 && hb2 == 1) {
            // Both subcarriers are ON! This is mathematically impossible 
            // for a single tag, meaning multiple tags are overlapping.
            return SLOT_COLLISION; 
        } 
        else { // hb1 == 0 && hb2 == 0
            // Reached an End of Frame (EOF) or a framing error.
            break; 
        }

        // Pack the decoded bit into our byte array (ISO 15693 is LSB-first)
        int byte_idx = decoded_bits / 8;
        int bit_in_byte = decoded_bits % 8;
        
        if (decoded_logic_bit) {
            decoded_bytes[byte_idx] |= (1 << bit_in_byte);
        }

        decoded_bits++;
        
        // Advance the window by one full Manchester bit (16 FIFO bits)
        current_bit_idx += (2 * BITS_PER_HALF_BIT); 
    }

    // 5. Verify the tag's CRC
    // -------------------------------------------------------------------------
    
    // If we didn't decode exactly 96 bits, the frame was incomplete
    if (decoded_bits < 96) {
        return SLOT_NO_RESPONSE; 
    }

    // The CRC is calculated over the first 10 bytes (Flags, DSFID, and UID)
    uint16_t calculated_crc = st25_calculate_crc16(decoded_bytes, 10);
    
    // The received CRC is in bytes 10 and 11 (LSB first)
    uint16_t received_crc = decoded_bytes[10] | (decoded_bytes[11] << 8);

    if (calculated_crc == received_crc) {
        // Validation success! Extract the 8-byte UID (Bytes 2 through 9)
        for (int i = 0; i < 8; i++) {
            decoded_uid[i] = decoded_bytes[2 + i];
        }
        return SLOT_VALID_TAG;
    }

    // If the CRC didn't match, the data was corrupted by noise (not a direct collision)
    ESP_LOGW("RX_DBG", "Decoded 96 bits, but CRC failed!");
    return SLOT_NO_RESPONSE; 
}

/**
 * @brief Slides a window across the FIFO data to find the ISO 15693 SOF pattern.
 * * @param data The raw FIFO byte array.
 * @param max_bytes The total size of the array.
 * @param sync_bit_index Pointer to store the bit index where the data payload starts.
 * @return true if SOF was found perfectly aligned, false otherwise.
 */
static bool st25_find_sof(const uint8_t *data, size_t max_bytes, size_t *sync_bit_index)
{
    // We need 8 Half-Bits to form the SOF.
    // Total window size = 8 Half-Bits * 8 bits/Half-Bit = 64 physical FIFO bits.
    size_t total_bits = max_bytes * 8;
    size_t sof_length_bits = 8 * BITS_PER_HALF_BIT; 
    
    // The strict ISO 15693 High-Data-Rate SOF pattern
    int expected_pattern[8] = {0, 0, 0, 1, 1, 1, 0, 1};

    // Slide the window 1 single FIFO bit at a time to perfectly phase-align
    for (size_t bit_idx = 0; bit_idx + sof_length_bits <= total_bits; bit_idx++) {
        
        bool match = true;
        
        // Check the 8 Half-Bits in this specific window position
        for (int hb = 0; hb < 8; hb++) {
            size_t hb_start = bit_idx + (hb * BITS_PER_HALF_BIT);
            int state = get_half_bit_state(data, max_bytes, hb_start);
            
            if (state != expected_pattern[hb]) {
                match = false;
                break; // Window doesn't match, break early and slide forward by 1 bit
            }
        }

        if (match) {
            // We found the SOF! The actual Manchester data payload starts exactly here.
            *sync_bit_index = bit_idx + sof_length_bits;
            return true;
        }
    }

    return false; // Reached the end of the FIFO without finding the SOF
}

/**
 * @brief Safely extracts a single bit from the raw FIFO byte array.
 * * @param data The raw FIFO byte array.
 * @param max_bytes The total size of the data array (to prevent buffer overflows).
 * @param bit_index The absolute index of the bit to extract (e.g., 0 to 767).
 * @return int 1 if the bit is set, 0 if it is clear, or -1 if out of bounds.
 */
static inline uint32_t get_fifo_bit(const uint8_t *data, size_t max_bytes, size_t bit_index) 
{
    size_t byte_idx = bit_index / 8;
    size_t bit_in_byte = bit_index % 8;

    if (byte_idx >= max_bytes) {
        return -1; // Out of bounds
    }

    // Assuming LSB-first packing for Stream Mode
    return (data[byte_idx] >> bit_in_byte) & 0x01;
}

static uint32_t get_half_bit_state(const uint8_t *data, size_t max_bytes, size_t start_bit_index) 
{
    int ones_count = 0;

    for (size_t i = 0; i < BITS_PER_HALF_BIT; i++) {
        int bit = get_fifo_bit(data, max_bytes, start_bit_index + i);
        if (bit == -1) {
            return -1; // Reached the end of the FIFO data
        }
        if (bit == 1) {
            ones_count++;
        }
    }

    // If 4 or more bits out of 8 are '1', the subcarrier was present (ON)
    if (ones_count >= (BITS_PER_HALF_BIT / 2)) {
        return 1;
    } else {
        return 0;
    }
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

esp_err_t send_16_slot_inventory(st25r3912_hw_t *hw)
{
    // Inventory command bytes: 
    // [0] Flags: 0x26 (High data rate, 16 slots, Inventory flag set)
    // [1] Command: 0x01 (Inventory command code)
    // [2] Mask Length: 0x00 (No mask)
    uint8_t inv_cmd[3] = { 0x26, 0x01, 0x00 };

    st25_send_iso15693_sof(hw);
    st25_send_iso15693_data(hw, inv_cmd, sizeof(inv_cmd));
    st25_send_iso15693_eof(hw);

    return ESP_OK;
}

/**
 * @brief Turns on the ST25R3912 antenna field (transmitter).
 * * @param hw Pointer to the hardware context.
 * @return esp_err_t ESP_OK on success.
 */
esp_err_t st25_field_on(st25r3912_hw_t *hw)
{
    // The tx_en bit is bit 3 (0x08) in the Operation Control Register (0x02).
    // We pass 0x08 as the mask, and 0x08 as the value to set it to 1.
    return st25_modify_register(hw, ST25R3912_REG_OP_CONTROL, 0x08, 0x08);
}

/**
 * @brief Turns off the ST25R3912 antenna field (transmitter).
 * * @param hw Pointer to the hardware context.
 * @return esp_err_t ESP_OK on success.
 */
esp_err_t st25_field_off(st25r3912_hw_t *hw)
{
    // We pass 0x08 as the mask, and 0x00 as the value to clear it to 0.
    return st25_modify_register(hw, ST25R3912_REG_OP_CONTROL, 0x08, 0x00);
}

esp_err_t st25_send_iso15693_sof(st25r3912_hw_t *hw) 
{
    // Pulse 1
    st25_write_register(hw, ST25R3912_REG_OP_CONTROL, OP_CTRL_TX_OFF);
    esp_rom_delay_us(1); // SPI latency + 1us = ~8-9us pulse
    st25_write_register(hw, ST25R3912_REG_OP_CONTROL, OP_CTRL_TX_ON);
    
    esp_rom_delay_us(26); // Unmodulated phase
    
    // Pulse 2
    st25_write_register(hw, ST25R3912_REG_OP_CONTROL, OP_CTRL_TX_OFF);
    esp_rom_delay_us(1); 
    st25_write_register(hw, ST25R3912_REG_OP_CONTROL, OP_CTRL_TX_ON);
    
    esp_rom_delay_us(26);
    return ESP_OK;
}

esp_err_t st25_send_iso15693_data(st25r3912_hw_t *hw, const uint8_t *data, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        uint8_t byte = data[i];
        for (int pair = 0; pair < 4; pair++) {
            uint8_t val = (byte >> (pair * 2)) & 0x03;
            for (int slot = 0; slot < 4; slot++) {
                if (slot == val) {
                    st25_write_register(hw, ST25R3912_REG_OP_CONTROL, OP_CTRL_TX_OFF);
                    esp_rom_delay_us(1);
                    st25_write_register(hw, ST25R3912_REG_OP_CONTROL, OP_CTRL_TX_ON);
                    esp_rom_delay_us(5); // Pad the remainder of the 18.88us window
                } else {
                    esp_rom_delay_us(18); // Empty slot wait time
                }
            }
        }
    }
    return ESP_OK;
}

esp_err_t st25_send_iso15693_eof(st25r3912_hw_t *hw)
{
    st25_write_register(hw, ST25R3912_REG_OP_CONTROL, OP_CTRL_TX_OFF);
    esp_rom_delay_us(1);
    st25_write_register(hw, ST25R3912_REG_OP_CONTROL, OP_CTRL_TX_ON);
    esp_rom_delay_us(26);
    
    // Clear FIFO of any TX garbage
    st25_execute_command(hw, 0xC2); 
    
    // Force the Receiver Ears ON immediately (using rx_man)
    st25_write_register(hw, ST25R3912_REG_OP_CONTROL, OP_CTRL_RX_ON);
    
    return ESP_OK;
}

void stack_push(anticollision_stack_t *stack, iso15693_mask_t mask)
{
    if (stack->top < 16) {
        stack->items[stack->top++] = mask;
    }
}

iso15693_mask_t stack_pop(anticollision_stack_t *stack)
{
    return stack->items[--stack->top];
}

/**
 * @brief Calculates the ISO/IEC 15693 CRC-16 over a data array.
 * * @param data Pointer to the byte array.
 * @param len Number of bytes to process.
 * @return uint16_t The calculated 16-bit CRC.
 */
uint16_t st25_calculate_crc16(const uint8_t *data, size_t len) 
{
    uint16_t crc = 0xFFFF; // Initial value mandated by ISO 15693

    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t bit = 0; bit < 8; bit++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0x8408; // 0x8408 is the reversed polynomial
            } else {
                crc >>= 1;
            }
        }
    }

    return ~crc; // Final XOR (bitwise inversion)
}

esp_err_t st25_test_anticollision(st25r3912_hw_t *hw)
{
    anticollision_stack_t stack = { .top = 0 };

    iso15693_mask_t initial_mask = {0};
    stack_push(&stack, initial_mask);

    iso15693_mask_t current_mask = stack_pop(&stack);

    st25_send_inventory_cmd(hw, current_mask);
}

/**
 * @brief Routes the internal oscillator to the MCU_CLK pin for diagnostic probing.
 * * @param hw Pointer to the hardware context.
 * @param enable If true, outputs a 6.78 MHz square wave to MCU_CLK. If false, sets pin to High-Z.
 * @return esp_err_t ESP_OK on success.
 */
esp_err_t st25_enable_mcu_clk_output(st25r3912_hw_t *hw, bool enable)
{
    if (hw == NULL) return ESP_ERR_INVALID_ARG;

    // The clock output routing is controlled by bits 1 and 0 (out_cl) 
    // in IO Configuration Register 1 (0x00).
    // Mask: 0x03 (0000 0011)
    // Value: 0x03 (11) outputs fc/2 (6.78 MHz), 0x00 disables it (High-Z).
    
    uint8_t mask = 0x03;
    uint8_t value = enable ? 0x03 : 0x00;
    
    return st25_modify_register(hw, ST25R3912_REG_IO_CONF_1, mask, value);
}