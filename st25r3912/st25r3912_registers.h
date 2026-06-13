#ifndef ST25R3912_REGISTERS_H
#define ST25R3912_REGISTERS_H

#include <stdint.h>

// -----------------------------------------------------------------------------
// ST25R3912 Register Address Map
// -----------------------------------------------------------------------------

// IO Configuration
#define ST25R3912_REG_IO_CONF_1                     0x00
#define ST25R3912_REG_IO_CONF_2                     0x01

// Operation Control & Mode Definition
#define ST25R3912_REG_OP_CONTROL                    0x02
#define ST25R3912_REG_MODE_DEF                      0x03
#define ST25R3912_REG_BIT_RATE_DEF                  0x04

// Protocol Settings
#define ST25R3912_REG_ISO14443A_NFC_SETTINGS        0x05
#define ST25R3912_REG_ISO14443B_SETTINGS_1          0x06
#define ST25R3912_REG_ISO14443B_FELICA_SETTINGS     0x07
#define ST25R3912_REG_STREAM_MODE_DEF               0x08
#define ST25R3912_REG_AUX_DEF                       0x09

// Receiver Configuration
#define ST25R3912_REG_RX_CONF_1                     0x0A
#define ST25R3912_REG_RX_CONF_2                     0x0B
#define ST25R3912_REG_RX_CONF_3                     0x0C
#define ST25R3912_REG_RX_CONF_4                     0x0D

// Timer Definition
#define ST25R3912_REG_MASK_RX_TIMER                 0x0E
#define ST25R3912_REG_NO_RESPONSE_TIMER_1           0x0F
#define ST25R3912_REG_NO_RESPONSE_TIMER_2           0x10
#define ST25R3912_REG_TIMER_CONTROL                 0x11
#define ST25R3912_REG_GENERAL_PURPOSE_TIMER_1       0x12
#define ST25R3912_REG_GENERAL_PURPOSE_TIMER_2       0x13

// Interrupt Mask Registers
#define ST25R3912_REG_MASK_MAIN_INT                 0x14
#define ST25R3912_REG_MASK_TIMER_NFC_INT            0x15
#define ST25R3912_REG_MASK_ERR_WAKEUP_INT           0x16

// Interrupt Status Registers (Read-Only)
#define ST25R3912_REG_MAIN_INT                      0x17
#define ST25R3912_REG_TIMER_NFC_INT                 0x18
#define ST25R3912_REG_ERR_WAKEUP_INT                0x19

// FIFO and Collision Registers (Read-Only)
#define ST25R3912_REG_FIFO_STATUS_1                 0x1A
#define ST25R3912_REG_FIFO_STATUS_2                 0x1B
#define ST25R3912_REG_COLLISION_DISPLAY             0x1C

// Transmission Registers
#define ST25R3912_REG_NUM_TX_BYTES_1                0x1D
#define ST25R3912_REG_NUM_TX_BYTES_2                0x1E

// NFCIP and ADC Display (Read-Only)
#define ST25R3912_REG_NFCIP_BIT_RATE_DISPLAY        0x1F
#define ST25R3912_REG_ADC_OUTPUT                    0x20

// Reserved
#define ST25R3912_REG_RESERVED_21                   0x21
#define ST25R3912_REG_RESERVED_22                   0x22
#define ST25R3912_REG_RESERVED_23                   0x23

// AM Modulation Depth & Antenna Driver
#define ST25R3912_REG_AM_MOD_DEPTH_CONTROL          0x24
#define ST25R3912_REG_AM_MOD_DEPTH_DISPLAY          0x25
#define ST25R3912_REG_RFO_AM_MOD_LEVEL_DEF          0x26
#define ST25R3912_REG_RFO_NORMAL_LEVEL_DEF          0x27

// Reserved
#define ST25R3912_REG_TX_DRIVER                     0x28

// External Field Detector & Regulator
#define ST25R3912_REG_EXT_FIELD_DETECTOR_THRESH     0x29
#define ST25R3912_REG_REGULATOR_VOLTAGE_CONTROL     0x2A
#define ST25R3912_REG_REGULATOR_TIMER_DISPLAY       0x2B
#define ST25R3912_REG_RSSI_DISPLAY                  0x2C
#define ST25R3912_REG_GAIN_REDUCTION_STATE          0x2D

// Reserved
#define ST25R3912_REG_RESERVED_2E                   0x2E
#define ST25R3912_REG_RESERVED_2F                   0x2F

// Auxiliary Display
#define ST25R3912_REG_AUX_DISPLAY                   0x30

// Wake-Up Timer & Measurements
#define ST25R3912_REG_WAKEUP_TIMER_CONTROL          0x31
#define ST25R3912_REG_AM_MEASURE_CONF               0x32
#define ST25R3912_REG_AM_MEASURE_REF                0x33
#define ST25R3912_REG_AM_MEASURE_AUTO_AVG_DISPLAY   0x34
#define ST25R3912_REG_AM_MEASURE_DISPLAY            0x35
#define ST25R3912_REG_PM_MEASURE_CONF               0x36
#define ST25R3912_REG_PM_MEASURE_REF                0x37
#define ST25R3912_REG_PM_MEASURE_AUTO_AVG_DISPLAY   0x38
#define ST25R3912_REG_PM_MEASURE_DISPLAY            0x39

// Reserved
#define ST25R3912_REG_RESERVED_3A                   0x3A
#define ST25R3912_REG_RESERVED_3B                   0x3B
#define ST25R3912_REG_RESERVED_3C                   0x3C
#define ST25R3912_REG_RESERVED_3D                   0x3D
#define ST25R3912_REG_RESERVED_3E                   0x3E

// IC Identity (Read-Only)
#define ST25R3912_REG_IC_IDENTITY                   0x3F

// Address 0x00: IO Configuration Register 1
typedef union {
    struct {
        uint8_t if_clk_off : 1; // Bit 0: 1: No LF clock on MCU_CLK
        uint8_t out_cl0    : 1; // Bit 1: MCU_CLK frequency selection
        uint8_t out_cl1    : 1; // Bit 2: MCU_CLK frequency selection
        uint8_t osc        : 1; // Bit 3: 0: 13.56 MHz Xtal; 1: 27.12 MHz Xtal
        uint8_t fifo_lt    : 1; // Bit 4: Tx FIFO water level (0: 32, 1: 16)
        uint8_t fifo_lr    : 1; // Bit 5: Rx FIFO water level (0: 64, 1: 80)
        uint8_t rfo2       : 1; // Bit 6: Single drive selection (0: RFO1, 1: RFO2)
        uint8_t single     : 1; // Bit 7: 1: Single antenna driving; 0: Differential
    } bits;
    uint8_t byte;
} io_config_reg_1_t;

// Address 0x01: IO Configuration Register 2
typedef union {
    struct {
        uint8_t slow_up    : 1; // Bit 0: 1: Slow ramp at Tx on
        uint8_t reserved_1 : 1; // Bit 1: Not used
        uint8_t io_18      : 1; // Bit 2: 1: Increase MISO driving level for 1.8V VDD_IO
        uint8_t miso_pd1   : 1; // Bit 3: 1: Pull-down on MISO when /SS is high
        uint8_t miso_pd2   : 1; // Bit 4: 1: Pull-down on MISO when /SS is low
        uint8_t reserved_5 : 1; // Bit 5: Not used
        uint8_t vspd_off   : 1; // Bit 6: 1: Disable VSP_D regulator
        uint8_t sup3V      : 1; // Bit 7: 0: 5 V supply; 1: 3.3 V supply
    } bits;
    uint8_t byte;
} io_config_reg_2_t;

// Address 0x02: Operation Control Register
typedef union {
    struct {
        uint8_t reserved_0 : 1; // Bit 0: Not used
        uint8_t reserved_1 : 1; // Bit 1: Not used
        uint8_t wu         : 1; // Bit 2: 1: Enables Wake-up mode
        uint8_t tx_en      : 1; // Bit 3: 1: Enables Tx operation
        uint8_t rx_man     : 1; // Bit 4: 0: Auto channel selection; 1: Manual selection
        uint8_t rx_chn     : 1; // Bit 5: 0: Both channels enabled; 1: One channel enabled
        uint8_t rx_en      : 1; // Bit 6: 1: Enables Rx operation
        uint8_t en         : 1; // Bit 7: 1: Enables oscillator and regulator
    } bits;
    uint8_t byte;
} operation_control_reg_t;

// Address 0x03: Mode Definition Register
typedef union {
    struct {
        uint8_t nfc_ar     : 1; // Bit 0: 1: Auto start Response RF collision avoidance
        uint8_t reserved_1 : 1; // Bit 1: Not used
        uint8_t reserved_2 : 1; // Bit 2: Not used
        uint8_t om0        : 1; // Bit 3: Operation mode selection bit 0
        uint8_t om1        : 1; // Bit 4: Operation mode selection bit 1
        uint8_t om2        : 1; // Bit 5: Operation mode selection bit 2
        uint8_t om3        : 1; // Bit 6: Operation mode selection bit 3
        uint8_t targ       : 1; // Bit 7: 0: Initiator; 1: Target
    } bits;
    uint8_t byte;
} mode_definition_reg_t;

// Address 0x04: Bit Rate Definition Register
typedef union {
    struct {
        uint8_t rx_rate0   : 1; // Bit 0: Selects bit rate for Rx
        uint8_t rx_rate1   : 1; // Bit 1: Selects bit rate for Rx
        uint8_t rx_rate2   : 1; // Bit 2: Selects bit rate for Rx
        uint8_t rx_rate3   : 1; // Bit 3: Selects bit rate for Rx
        uint8_t tx_rate0   : 1; // Bit 4: Selects bit rate for Tx
        uint8_t tx_rate1   : 1; // Bit 5: Selects bit rate for Tx
        uint8_t tx_rate2   : 1; // Bit 6: Selects bit rate for Tx
        uint8_t tx_rate3   : 1; // Bit 7: Selects bit rate for Tx
    } bits;
    uint8_t byte;
} bit_rate_definition_reg_t;

// Address 0x05: ISO14443A and NFC 106kb/s Settings Register
typedef union {
    struct {
        uint8_t antcl      : 1; // Bit 0: 1: ISO14443 anticollision frame
        uint8_t p_len0     : 1; // Bit 1: Modulation pulse width
        uint8_t p_len1     : 1; // Bit 2: Modulation pulse width
        uint8_t p_len2     : 1; // Bit 3: Modulation pulse width
        uint8_t p_len3     : 1; // Bit 4: Modulation pulse width
        uint8_t nfc_f0     : 1; // Bit 5: 1: Support of NFCIP-1 Transport frame format
        uint8_t no_rx_par  : 1; // Bit 6: 1: Receive without parity and CRC
        uint8_t no_tx_par  : 1; // Bit 7: 1: No parity bit is generated during Tx
    } bits;
    uint8_t byte;
} iso14443a_nfc_settings_reg_t;

// Address 0x06: ISO14443B Settings Register 1
typedef union {
    struct {
        uint8_t rx_st_om   : 1; // Bit 0: 1: Start/stop bit omission for Rx
        uint8_t half       : 1; // Bit 1: 1: Sets SOF/EOF settings to middle of spec
        uint8_t eof        : 1; // Bit 2: EOF length
        uint8_t sof_1      : 1; // Bit 3: SOF number of etu with logic 1
        uint8_t sof_0      : 1; // Bit 4: SOF number of etu with logic 0
        uint8_t egt0       : 1; // Bit 5: EGT defined in number of etu
        uint8_t egt1       : 1; // Bit 6: EGT defined in number of etu
        uint8_t egt2       : 1; // Bit 7: EGT defined in number of etu
    } bits;
    uint8_t byte;
} iso14443b_settings_reg_1_t;

// Address 0x07: ISO14443B and FeliCa Settings Register
typedef union {
    struct {
        uint8_t f_p0       : 1; // Bit 0: FeliCa preamble length
        uint8_t f_p1       : 1; // Bit 1: FeliCa preamble length
        uint8_t phc_th     : 1; // Bit 2: 1: Increased tolerance of phase change detection
        uint8_t eof_12     : 1; // Bit 3: 1: PICC EOF 10 to 12 etu
        uint8_t no_eof     : 1; // Bit 4: 1: No EOF PICC to PCD
        uint8_t no_sof     : 1; // Bit 5: 1: No SOF PICC to PCD
        uint8_t tr1_0      : 1; // Bit 6: Minimum TR1 for a PICC to PCD bit rate
        uint8_t tr1_1      : 1; // Bit 7: Minimum TR1 for a PICC to PCD bit rate
    } bits;
    uint8_t byte;
} iso14443b_felica_settings_reg_t;

// Address 0x08: Stream Mode Definition Register
typedef union {
    struct {
        uint8_t stx0       : 1; // Bit 0: Definition of time period for Tx modulator
        uint8_t stx1       : 1; // Bit 1: Definition of time period for Tx modulator
        uint8_t stx2       : 1; // Bit 2: Definition of time period for Tx modulator
        uint8_t scp0       : 1; // Bit 3: Number of sub-carrier pulses in report period
        uint8_t scp1       : 1; // Bit 4: Number of sub-carrier pulses in report period
        uint8_t scf0       : 1; // Bit 5: Sub-carrier frequency definition
        uint8_t scf1       : 1; // Bit 6: Sub-carrier frequency definition
        uint8_t reserved_7 : 1; // Bit 7: Not used
    } bits;
    uint8_t byte;
} stream_mode_definition_reg_t;

// Address 0x09: Auxiliary Definition Register
typedef union {
    struct {
        uint8_t nfc_n0     : 1; // Bit 0: Value of n for NFC field ON (0..3)
        uint8_t nfc_n1     : 1; // Bit 1: Value of n for NFC field ON (0..3)
        uint8_t rx_tol     : 1; // Bit 2: 1: More tolerant BPSK decoder/first byte
        uint8_t ook_hr     : 1; // Bit 3: 1: Put RFO driver in tristate during OOK
        uint8_t en_fd      : 1; // Bit 4: 1: Enable External field detector
        uint8_t tr_am      : 1; // Bit 5: 0: OOK; 1: AM modulation
        uint8_t crc_2_fifo : 1; // Bit 6: 1: Put CRC bytes in FIFO
        uint8_t no_crc_rx  : 1; // Bit 7: 1: Receive without CRC
    } bits;
    uint8_t byte;
} auxiliary_definition_reg_t;

// Address 0x0A: Receiver Configuration Register 1
typedef union {
    struct {
        uint8_t z12k       : 1; // Bit 0: First and third stage zero setting
        uint8_t h80        : 1; // Bit 1: First and third stage zero setting
        uint8_t h200       : 1; // Bit 2: First and third stage zero setting
        uint8_t lp0        : 1; // Bit 3: Low pass control
        uint8_t lp1        : 1; // Bit 4: Low pass control
        uint8_t lp2        : 1; // Bit 5: Low pass control
        uint8_t amd_sel    : 1; // Bit 6: AM demodulator type (0: Peak detector, 1: Mixer)
        uint8_t ch_sel     : 1; // Bit 7: 0: Enable AM channel; 1: Enable PM channel
    } bits;
    uint8_t byte;
} receiver_config_reg_1_t;

// Address 0x0B: Receiver Configuration Register 2
typedef union {
    struct {
        uint8_t pmix_cl    : 1; // Bit 0: PM demodulator mixer clock source
        uint8_t sqm_dyn    : 1; // Bit 1: 1: Auto squelch activation after Tx
        uint8_t agc_alg    : 1; // Bit 2: AGC algorithm (0: preset, 1: reset)
        uint8_t agc_m      : 1; // Bit 3: AGC operates on first eight pulses or all
        uint8_t agc_en     : 1; // Bit 4: 1: AGC is enabled
        uint8_t lf_en      : 1; // Bit 5: 1: LF signal on receiver input
        uint8_t lf_op      : 1; // Bit 6: 0: Diff LF operation; 1: LF input split
        uint8_t rx_lp      : 1; // Bit 7: 1: Low power receiver operation
    } bits;
    uint8_t byte;
} receiver_config_reg_2_t;

// Address 0x0C: Receiver Configuration Register 3
typedef union {
    struct {
        uint8_t rg_nfc     : 1; // Bit 0: 1: Forces gain reduction to 6 dB
        uint8_t lim        : 1; // Bit 1: 1: Clip output of 1st and 2nd stage
        uint8_t rg1_pm0    : 1; // Bit 2: Gain reduction/boost in PM channel 1st stage
        uint8_t rg1_pm1    : 1; // Bit 3: Gain reduction/boost in PM channel 1st stage
        uint8_t rg1_pm2    : 1; // Bit 4: Gain reduction/boost in PM channel 1st stage
        uint8_t rg1_am0    : 1; // Bit 5: Gain reduction/boost in AM channel 1st stage
        uint8_t rg1_am1    : 1; // Bit 6: Gain reduction/boost in AM channel 1st stage
        uint8_t rg1_am2    : 1; // Bit 7: Gain reduction/boost in AM channel 1st stage
    } bits;
    uint8_t byte;
} receiver_config_reg_3_t;

// Address 0x0D: Receiver Configuration Register 4
typedef union {
    struct {
        uint8_t rg2_pm0    : 1; // Bit 0: PM channel 2nd/3rd stage and digitizer gain
        uint8_t rg2_pm1    : 1; // Bit 1: PM channel 2nd/3rd stage and digitizer gain
        uint8_t rg2_pm2    : 1; // Bit 2: PM channel 2nd/3rd stage and digitizer gain
        uint8_t rg2_pm3    : 1; // Bit 3: PM channel 2nd/3rd stage and digitizer gain
        uint8_t rg2_am0    : 1; // Bit 4: AM channel 2nd/3rd stage and digitizer gain
        uint8_t rg2_am1    : 1; // Bit 5: AM channel 2nd/3rd stage and digitizer gain
        uint8_t rg2_am2    : 1; // Bit 6: AM channel 2nd/3rd stage and digitizer gain
        uint8_t rg2_am3    : 1; // Bit 7: AM channel 2nd/3rd stage and digitizer gain
    } bits;
    uint8_t byte;
} receiver_config_reg_4_t;

// Address 0x0E: Mask Receive Timer Register
typedef union {
    uint8_t byte;               // All bits: mrt0 to mrt7
} mask_receive_timer_reg_t;

// Address 0x0F: No-Response Timer Register 1
typedef union {
    uint8_t byte;               // MSB bits: nrt8 to nrt15
} no_response_timer_reg_1_t;

// Address 0x10: No-Response Timer Register 2
typedef union {
    uint8_t byte;               // LSB bits: nrt0 to nrt7
} no_response_timer_reg_2_t;

// Address 0x11: General Purpose and No-Response Timer Control Register
typedef union {
    struct {
        uint8_t nrt_step   : 1; // Bit 0: 0: 64/fc; 1: 4096/fc
        uint8_t nrt_emv    : 1; // Bit 1: 1: EMV mode of No-response timer
        uint8_t reserved_2 : 1; // Bit 2: Not used
        uint8_t reserved_3 : 1; // Bit 3: Not used
        uint8_t reserved_4 : 1; // Bit 4: Not used
        uint8_t gptc0      : 1; // Bit 5: Timer trigger source
        uint8_t gptc1      : 1; // Bit 6: Timer trigger source
        uint8_t gptc2      : 1; // Bit 7: Timer trigger source
    } bits;
    uint8_t byte;
} timer_control_reg_t;

// Address 0x12: General Purpose Timer Register 1
typedef union {
    uint8_t byte;               // MSB bits: gpt8 to gpt15
} general_purpose_timer_reg_1_t;

// Address 0x13: General Purpose Timer Register 2
typedef union {
    uint8_t byte;               // LSB bits: gpt0 to gpt7
} general_purpose_timer_reg_2_t;

// Address 0x14: Mask Main Interrupt Register
typedef union {
    struct {
        uint8_t reserved_0 : 1; // Bit 0: Not used
        uint8_t reserved_1 : 1; // Bit 1: Not used
        uint8_t M_col      : 1; // Bit 2: Mask IRQ due to bit collision
        uint8_t M_txe      : 1; // Bit 3: Mask IRQ due to end of transmission
        uint8_t M_rxe      : 1; // Bit 4: Mask IRQ due to end of receive
        uint8_t M_rxs      : 1; // Bit 5: Mask IRQ due to start of receive
        uint8_t M_wl       : 1; // Bit 6: Mask IRQ due to FIFO water level
        uint8_t M_osc      : 1; // Bit 7: Mask IRQ when oscillator freq is stable
    } bits;
    uint8_t byte;
} mask_main_interrupt_reg_t;

// Address 0x15: Mask Timer and NFC Interrupt Register
typedef union {
    struct {
        uint8_t M_nfct     : 1; // Bit 0: Mask IRQ initiator bit rate recognized
        uint8_t M_cat      : 1; // Bit 1: Mask IRQ after min guard time expire
        uint8_t M_cac      : 1; // Bit 2: Mask IRQ collision during RF avoidance
        uint8_t M_eof      : 1; // Bit 3: Mask IRQ external field drop below target
        uint8_t M_eon      : 1; // Bit 4: Mask IRQ external field higher than target
        uint8_t M_gpe      : 1; // Bit 5: Mask IRQ general purpose timer expire
        uint8_t M_nre      : 1; // Bit 6: Mask IRQ No-response timer expire
        uint8_t M_dct      : 1; // Bit 7: Mask IRQ termination of direct command
    } bits;
    uint8_t byte;
} mask_timer_nfc_interrupt_reg_t;

// Address 0x16: Mask Error and Wake-Up Interrupt Register
typedef union {
    struct {
        uint8_t M_wcap     : 1; // Bit 0: Mask Wake-up IRQ capacitance measure
        uint8_t M_wph      : 1; // Bit 1: Mask Wake-up IRQ phase measure
        uint8_t M_wam      : 1; // Bit 2: Mask Wake-up IRQ amplitude measure
        uint8_t M_wt       : 1; // Bit 3: Mask IRQ wake-up timer interrupt
        uint8_t M_err1     : 1; // Bit 4: Mask IRQ hard framing error
        uint8_t M_err2     : 1; // Bit 5: Mask IRQ soft framing error
        uint8_t M_par      : 1; // Bit 6: Mask IRQ parity error
        uint8_t M_crc      : 1; // Bit 7: Mask IRQ CRC error
    } bits;
    uint8_t byte;
} mask_err_wakeup_interrupt_reg_t;

// Address 0x17: Main Interrupt Register (Read-Only)
typedef union {
    struct {
        uint8_t I_err      : 1; // Bit 0: IRQ due to error and wake-up
        uint8_t I_tim      : 1; // Bit 1: IRQ due to timer or NFC event
        uint8_t I_col      : 1; // Bit 2: IRQ due to bit collision
        uint8_t I_txe      : 1; // Bit 3: IRQ due to end of transmission
        uint8_t I_rxe      : 1; // Bit 4: IRQ due to end of receive
        uint8_t I_rxs      : 1; // Bit 5: IRQ due to start of receive
        uint8_t I_wl       : 1; // Bit 6: IRQ due to FIFO water level
        uint8_t I_osc      : 1; // Bit 7: IRQ when oscillator freq is stable
    } bits;
    uint8_t byte;
} main_interrupt_reg_t;

// Address 0x18: Timer and NFC Interrupt Register (Read-Only)
typedef union {
    struct {
        uint8_t I_nfct     : 1; // Bit 0: IRQ initiator bit rate recognized
        uint8_t I_cat      : 1; // Bit 1: IRQ after min guard time expire
        uint8_t I_cac      : 1; // Bit 2: IRQ collision during RF collision avoidance
        uint8_t I_eof      : 1; // Bit 3: IRQ external field drop below target
        uint8_t I_eon      : 1; // Bit 4: IRQ external field higher than target
        uint8_t I_gpe      : 1; // Bit 5: IRQ general purpose timer expire
        uint8_t I_nre      : 1; // Bit 6: IRQ No-Response Timer expire
        uint8_t I_dct      : 1; // Bit 7: IRQ termination of direct command
    } bits;
    uint8_t byte;
} timer_nfc_interrupt_reg_t;

// Address 0x19: Error and Wake-Up Interrupt Register (Read-Only)
typedef union {
    struct {
        uint8_t I_wcap     : 1; // Bit 0: Wake-up interrupt capacitance measure
        uint8_t I_wph      : 1; // Bit 1: Wake-up interrupt phase measure
        uint8_t I_wam      : 1; // Bit 2: Wake-up interrupt amplitude measure
        uint8_t I_wt       : 1; // Bit 3: Wake-up timer interrupt
        uint8_t I_err1     : 1; // Bit 4: Hard framing error
        uint8_t I_err2     : 1; // Bit 5: Soft framing error
        uint8_t I_par      : 1; // Bit 6: Parity error
        uint8_t I_crc      : 1; // Bit 7: CRC error
    } bits;
    uint8_t byte;
} error_wakeup_interrupt_reg_t;

// Address 0x1A: FIFO Status Register 1 (Read-Only)
typedef union {
    struct {
        uint8_t fifo_b0    : 1; // Bit 0: Number of unread bytes in FIFO
        uint8_t fifo_b1    : 1; // Bit 1: Number of unread bytes in FIFO
        uint8_t fifo_b2    : 1; // Bit 2: Number of unread bytes in FIFO
        uint8_t fifo_b3    : 1; // Bit 3: Number of unread bytes in FIFO
        uint8_t fifo_b4    : 1; // Bit 4: Number of unread bytes in FIFO
        uint8_t fifo_b5    : 1; // Bit 5: Number of unread bytes in FIFO
        uint8_t fifo_b6    : 1; // Bit 6: Number of unread bytes in FIFO
        uint8_t reserved_7 : 1; // Bit 7: Not used
    } bits;
    uint8_t byte;
} fifo_status_reg_1_t;

// Address 0x1B: FIFO Status Register 2 (Read-Only)
typedef union {
    struct {
        uint8_t np_lb      : 1; // Bit 0: 1: Parity bit is missing in last byte
        uint8_t fifo_lb0   : 1; // Bit 1: Bits in the last FIFO byte if incomplete
        uint8_t fifo_lb1   : 1; // Bit 2: Bits in the last FIFO byte if incomplete
        uint8_t fifo_lb2   : 1; // Bit 3: Bits in the last FIFO byte if incomplete
        uint8_t fifo_ncp   : 1; // Bit 4: 1: Last FIFO byte is not complete
        uint8_t fifo_ovr   : 1; // Bit 5: 1: FIFO overflow
        uint8_t fifo_unf   : 2; // Bits 6-7: 1: FIFO underflow
    } bits;
    uint8_t byte;
} fifo_status_reg_2_t;

// Address 0x1C: Collision Display Register (Read-Only)
typedef union {
    struct {
        uint8_t c_pb       : 1; // Bit 0: 1: Collision in parity bit
        uint8_t c_bit0     : 1; // Bit 1: Number of bits before collision
        uint8_t c_bit1     : 1; // Bit 2: Number of bits before collision
        uint8_t c_bit2     : 1; // Bit 3: Number of bits before collision
        uint8_t c_byte0    : 1; // Bit 4: Number of full bytes before collision
        uint8_t c_byte1    : 1; // Bit 5: Number of full bytes before collision
        uint8_t c_byte2    : 1; // Bit 6: Number of full bytes before collision
        uint8_t c_byte3    : 1; // Bit 7: Number of full bytes before collision
    } bits;
    uint8_t byte;
} collision_display_reg_t;

// Address 0x1D: Number of Transmitted Bytes Register 1
typedef union {
    struct {
        uint8_t ntx5       : 1; // Bit 0: Number of full bytes to transmit (MSB)
        uint8_t ntx6       : 1; // Bit 1: ...
        uint8_t ntx7       : 1; // Bit 2: ...
        uint8_t ntx8       : 1; // Bit 3: ...
        uint8_t ntx9       : 1; // Bit 4: ...
        uint8_t ntx10      : 1; // Bit 5: ...
        uint8_t ntx11      : 1; // Bit 6: ...
        uint8_t ntx12      : 1; // Bit 7: Number of full bytes to transmit (MSB)
    } bits;
    uint8_t byte;
} num_tx_bytes_reg_1_t;

// Address 0x1E: Number of Transmitted Bytes Register 2
typedef union {
    struct {
        uint8_t nbtx0      : 1; // Bit 0: Number of bits in the split byte
        uint8_t nbtx1      : 1; // Bit 1: Number of bits in the split byte
        uint8_t nbtx2      : 1; // Bit 2: Number of bits in the split byte
        uint8_t ntx0       : 1; // Bit 3: Number of full bytes to transmit (LSB)
        uint8_t ntx1       : 1; // Bit 4: ...
        uint8_t ntx2       : 1; // Bit 5: ...
        uint8_t ntx3       : 1; // Bit 6: ...
        uint8_t ntx4       : 1; // Bit 7: Number of full bytes to transmit (LSB)
    } bits;
    uint8_t byte;
} num_tx_bytes_reg_2_t;

// Address 0x1F: NFCIP Bit Rate Detection Display Register (Read-Only)
typedef union {
    struct {
        uint8_t reserved_0 : 1; // Bit 0: Not used
        uint8_t reserved_1 : 1; // Bit 1: Not used
        uint8_t reserved_2 : 1; // Bit 2: Not used
        uint8_t reserved_3 : 1; // Bit 3: Not used
        uint8_t nfc_rate0  : 1; // Bit 4: Auto detected bit rate
        uint8_t nfc_rate1  : 1; // Bit 5: Auto detected bit rate
        uint8_t nfc_rate2  : 1; // Bit 6: Auto detected bit rate
        uint8_t nfc_rate3  : 1; // Bit 7: Auto detected bit rate
    } bits;
    uint8_t byte;
} nfcip_bit_rate_display_reg_t;

// Address 0x20: A/D Converter Output Register (Read-Only)
typedef union {
    uint8_t byte;               // Bits ad0 to ad7 display A/D conversion result
} adc_output_reg_t;

// Address 0x24: AM Modulation Depth Control Register
typedef union {
    struct {
        uint8_t reserved_0 : 1; // Bit 0: Not used
        uint8_t mod0       : 1; // Bit 1: Target AM modulation depth
        uint8_t mod1       : 1; // Bit 2: Target AM modulation depth
        uint8_t mod2       : 1; // Bit 3: Target AM modulation depth
        uint8_t mod3       : 1; // Bit 4: Target AM modulation depth
        uint8_t mod4       : 1; // Bit 5: Target AM modulation depth
        uint8_t mod5       : 1; // Bit 6: Target AM modulation depth
        uint8_t am_s       : 1; // Bit 7: 0: Calibrate defined; 1: Register defined
    } bits;
    uint8_t byte;
} am_mod_depth_control_reg_t;

// Address 0x25: AM Modulation Depth Display Register (Read-Only)
typedef union {
    uint8_t byte;               // Bits md_0 to md_7
} am_mod_depth_display_reg_t;

// Address 0x26: RFO AM Modulated Level Definition Register
typedef union {
    struct {
        uint8_t dram0      : 1; // Bit 0: 256 Ohm segment disable
        uint8_t dram1      : 1; // Bit 1: 128 Ohm segment disable
        uint8_t dram2      : 1; // Bit 2: 64 Ohm segment disable
        uint8_t dram3      : 1; // Bit 3: 32 Ohm segment disable
        uint8_t dram4      : 1; // Bit 4: 16 Ohm segment disable
        uint8_t dram5      : 1; // Bit 5: 8 Ohm segment disable
        uint8_t dram6      : 1; // Bit 6: 4 Ohm segment disable
        uint8_t dram7      : 1; // Bit 7: 2 Ohm segment disable
    } bits;
    uint8_t byte;
} rfo_am_mod_level_def_reg_t;

// Address 0x27: RFO Normal Level Definition Register
typedef union {
    struct {
        uint8_t droff0     : 1; // Bit 0: 256 Ohm segment disable
        uint8_t droff1     : 1; // Bit 1: 128 Ohm segment disable
        uint8_t droff2     : 1; // Bit 2: 64 Ohm segment disable
        uint8_t droff3     : 1; // Bit 3: 32 Ohm segment disable
        uint8_t droff4     : 1; // Bit 4: 16 Ohm segment disable
        uint8_t droff5     : 1; // Bit 5: 8 Ohm segment disable
        uint8_t droff6     : 1; // Bit 6: 4 Ohm segment disable
        uint8_t droff7     : 1; // Bit 7: 2 Ohm segment disable
    } bits;
    uint8_t byte;
} rfo_normal_level_def_reg_t;

// Address 0x29: External Field Detector Threshold Register
typedef union {
    struct {
        uint8_t rfe_t0     : 1; // Bit 0: Collision avoidance threshold
        uint8_t rfe_t1     : 1; // Bit 1: Collision avoidance threshold
        uint8_t rfe_t2     : 1; // Bit 2: Collision avoidance threshold
        uint8_t rfe_t3     : 1; // Bit 3: Collision avoidance threshold
        uint8_t trg_l0     : 1; // Bit 4: Peer detection threshold
        uint8_t trg_l1     : 1; // Bit 5: Peer detection threshold
        uint8_t trg_l2     : 1; // Bit 6: Peer detection threshold
        uint8_t reserved_7 : 1; // Bit 7: Not used
    } bits;
    uint8_t byte;
} ext_field_detector_thresh_reg_t;

// Address 0x2A: Regulator Voltage Control Register
typedef union {
    struct {
        uint8_t reserved_0 : 1; // Bit 0: Not used
        uint8_t mpsv0      : 1; // Bit 1: Direct command Measure power supply source
        uint8_t mpsv1      : 1; // Bit 2: Direct command Measure power supply source
        uint8_t rege_0     : 1; // Bit 3: External definition of regulated voltage
        uint8_t rege_1     : 1; // Bit 4: External definition of regulated voltage
        uint8_t rege_2     : 1; // Bit 5: External definition of regulated voltage
        uint8_t rege_3     : 1; // Bit 6: External definition of regulated voltage
        uint8_t reg_s      : 1; // Bit 7: 0: Auto (Adjust cmd); 1: Register defined
    } bits;
    uint8_t byte;
} regulator_voltage_control_reg_t;

// Address 0x2B: Regulator and Timer Display Register (Read-Only)
typedef union {
    struct {
        uint8_t mrt_on     : 1; // Bit 0: 1: Mask receive timer is running
        uint8_t nrt_on     : 1; // Bit 1: 1: No-response timer is running
        uint8_t gpt_on     : 1; // Bit 2: 1: General purpose timer is running
        uint8_t reserved_3 : 1; // Bit 3: Not used
        uint8_t reg_0      : 1; // Bit 4: Actual regulated voltage setting
        uint8_t reg_1      : 1; // Bit 5: Actual regulated voltage setting
        uint8_t reg_2      : 1; // Bit 6: Actual regulated voltage setting
        uint8_t reg_3      : 1; // Bit 7: Actual regulated voltage setting
    } bits;
    uint8_t byte;
} reg_timer_display_reg_t;

// Address 0x2C: RSSI Display Register (Read-Only)
typedef union {
    struct {
        uint8_t rssi_pm_0  : 1; // Bit 0: PM channel RSSI peak value
        uint8_t rssi_pm_1  : 1; // Bit 1: PM channel RSSI peak value
        uint8_t rssi_pm_2  : 1; // Bit 2: PM channel RSSI peak value
        uint8_t rssi_pm_3  : 1; // Bit 3: PM channel RSSI peak value
        uint8_t rssi_am_0  : 1; // Bit 4: AM channel RSSI peak value
        uint8_t rssi_am_1  : 1; // Bit 5: AM channel RSSI peak value
        uint8_t rssi_am_2  : 1; // Bit 6: AM channel RSSI peak value
        uint8_t rssi_am_3  : 1; // Bit 7: AM channel RSSI peak value
    } bits;
    uint8_t byte;
} rssi_display_reg_t;

// Address 0x2D: Gain Reduction State Register (Read-Only)
typedef union {
    struct {
        uint8_t gs_pm_0    : 1; // Bit 0: PM channel gain reduction
        uint8_t gs_pm_1    : 1; // Bit 1: PM channel gain reduction
        uint8_t gs_pm_2    : 1; // Bit 2: PM channel gain reduction
        uint8_t gs_pm_3    : 1; // Bit 3: PM channel gain reduction
        uint8_t gs_am_0    : 1; // Bit 4: AM channel gain reduction
        uint8_t gs_am_1    : 1; // Bit 5: AM channel gain reduction
        uint8_t gs_am_2    : 1; // Bit 6: AM channel gain reduction
        uint8_t gs_am_3    : 1; // Bit 7: AM channel gain reduction
    } bits;
    uint8_t byte;
} gain_reduction_state_reg_t;

// Address 0x30: Auxiliary Display Register (Read-Only)
typedef union {
    struct {
        uint8_t en_ac      : 1; // Bit 0: 1: Ext field detector active in RF collision avoid
        uint8_t nfc_t      : 1; // Bit 1: 1: Ext field detector active in peer detection
        uint8_t rx_act     : 1; // Bit 2: 1: Receive coder is receiving a message
        uint8_t rx_on      : 1; // Bit 3: 1: Receive coder is enabled
        uint8_t osc_ok     : 1; // Bit 4: 1: Xtal oscillation is stable
        uint8_t tx_on      : 1; // Bit 5: 1: Transmission is active
        uint8_t efd_o      : 1; // Bit 6: 1: External field detected
        uint8_t a_cha      : 1; // Bit 7: 0: AM; 1: PM Currently selected channel
    } bits;
    uint8_t byte;
} auxiliary_display_reg_t;

// Address 0x31: Wake-Up Timer Control Register
typedef union {
    struct {
        uint8_t reserved_0 : 1; // Bit 0: Not used
        uint8_t wph        : 1; // Bit 1: 1: Perform phase measurement at timeout
        uint8_t wam        : 1; // Bit 2: 1: Perform amplitude measurement at timeout
        uint8_t wto        : 1; // Bit 3: 1: IRQ at every timeout
        uint8_t wut0       : 1; // Bit 4: Wake-up timer timeout value
        uint8_t wut1       : 1; // Bit 5: Wake-up timer timeout value
        uint8_t wut2       : 1; // Bit 6: Wake-up timer timeout value
        uint8_t wur        : 1; // Bit 7: 0: 100 ms; 1: 10 ms Wake-up timer range
    } bits;
    uint8_t byte;
} wake_up_timer_control_reg_t;

// Address 0x32: Amplitude Measurement Configuration Register
typedef union {
    struct {
        uint8_t am_ae      : 1; // Bit 0: 0: Reference register; 1: Auto-averaging ref
        uint8_t am_aew1    : 1; // Bit 1: Weight of last measurement result
        uint8_t am_aew2    : 1; // Bit 2: Weight of last measurement result
        uint8_t am_aam     : 1; // Bit 3: 1: Include the IRQ measurement in average
        uint8_t am_d0      : 1; // Bit 4: Difference to reference that triggers IRQ
        uint8_t am_d1      : 1; // Bit 5: Difference to reference that triggers IRQ
        uint8_t am_d2      : 1; // Bit 6: Difference to reference that triggers IRQ
        uint8_t am_d3      : 1; // Bit 7: Difference to reference that triggers IRQ
    } bits;
    uint8_t byte;
} am_measure_config_reg_t;

// Address 0x33: Amplitude Measurement Reference Register
typedef union {
    uint8_t byte;               // Bits am_ref0 to am_ref7
} am_measure_ref_reg_t;

// Address 0x34: Amplitude Measurement Auto-Averaging Display Register (Read-Only)
typedef union {
    uint8_t byte;               // Bits amd_aad0 to amd_aad7
} am_measure_auto_avg_display_reg_t;

// Address 0x35: Amplitude Measurement Display Register (Read-Only)
typedef union {
    uint8_t byte;               // Bits am_amd0 to am_amd7
} am_measure_display_reg_t;

// Address 0x36: Phase Measurement Configuration Register
typedef union {
    struct {
        uint8_t pm_ae      : 1; // Bit 0: 0: Reference register; 1: Auto-averaging ref
        uint8_t pm_aew0    : 1; // Bit 1: Weight of last measurement result
        uint8_t pm_aew1    : 1; // Bit 2: Weight of last measurement result
        uint8_t pm_aam     : 1; // Bit 3: 1: Include the IRQ measurement in average
        uint8_t pm_d0      : 1; // Bit 4: Difference to reference that triggers IRQ
        uint8_t pm_d1      : 1; // Bit 5: Difference to reference that triggers IRQ
        uint8_t pm_d2      : 1; // Bit 6: Difference to reference that triggers IRQ
        uint8_t pm_d3      : 1; // Bit 7: Difference to reference that triggers IRQ
    } bits;
    uint8_t byte;
} phase_measure_config_reg_t;

// Address 0x37: Phase Measurement Reference Register
typedef union {
    uint8_t byte;               // Bits pm_ref0 to pm_ref7
} phase_measure_ref_reg_t;

// Address 0x38: Phase Measurement Auto-Averaging Display Register (Read-Only)
typedef union {
    uint8_t byte;               // Bits pm_aad0 to pm_aad7
} phase_measure_auto_avg_display_reg_t;

// Address 0x39: Phase Measurement Display Register (Read-Only)
typedef union {
    uint8_t byte;               // Bits pm_amd0 to pm_amd7
} phase_measure_display_reg_t;

// Address 0x3F: IC Identity Register (Read-Only)
typedef union {
    struct {
        uint8_t ic_rev0    : 1; // Bit 0: IC revision code
        uint8_t ic_rev1    : 1; // Bit 1: IC revision code
        uint8_t ic_rev2    : 1; // Bit 2: IC revision code
        uint8_t ic_type0   : 1; // Bit 3: IC type code
        uint8_t ic_type1   : 1; // Bit 4: IC type code
        uint8_t ic_type2   : 1; // Bit 5: IC type code
        uint8_t ic_type3   : 1; // Bit 6: IC type code
        uint8_t ic_type4   : 1; // Bit 7: IC type code
    } bits;
    uint8_t byte;
} ic_identity_reg_t;



typedef struct {
    io_config_reg_1_t                    io_config_1;                    // 0x00
    io_config_reg_2_t                    io_config_2;                    // 0x01
    operation_control_reg_t              operation_control;              // 0x02
    mode_definition_reg_t                mode_definition;                // 0x03
    bit_rate_definition_reg_t            bit_rate_definition;            // 0x04
    iso14443a_nfc_settings_reg_t         iso14443a_nfc_settings;         // 0x05
    iso14443b_settings_reg_1_t           iso14443b_settings_1;           // 0x06
    iso14443b_felica_settings_reg_t      iso14443b_felica_settings;      // 0x07
    stream_mode_definition_reg_t         stream_mode_definition;         // 0x08
    auxiliary_definition_reg_t           auxiliary_definition;           // 0x09
    receiver_config_reg_1_t              receiver_config_1;              // 0x0A
    receiver_config_reg_2_t              receiver_config_2;              // 0x0B
    receiver_config_reg_3_t              receiver_config_3;              // 0x0C
    receiver_config_reg_4_t              receiver_config_4;              // 0x0D
    mask_receive_timer_reg_t             mask_receive_timer;             // 0x0E
    no_response_timer_reg_1_t            no_response_timer_1;            // 0x0F
    no_response_timer_reg_2_t            no_response_timer_2;            // 0x10
    timer_control_reg_t                  timer_control;                  // 0x11
    general_purpose_timer_reg_1_t        general_purpose_timer_1;        // 0x12
    general_purpose_timer_reg_2_t        general_purpose_timer_2;        // 0x13
    mask_main_interrupt_reg_t            mask_main_interrupt;            // 0x14
    mask_timer_nfc_interrupt_reg_t       mask_timer_nfc_interrupt;       // 0x15
    mask_err_wakeup_interrupt_reg_t      mask_err_wakeup_interrupt;      // 0x16
    main_interrupt_reg_t                 main_interrupt;                 // 0x17
    timer_nfc_interrupt_reg_t            timer_nfc_interrupt;            // 0x18
    error_wakeup_interrupt_reg_t         error_wakeup_interrupt;         // 0x19
    fifo_status_reg_1_t                  fifo_status_1;                  // 0x1A
    fifo_status_reg_2_t                  fifo_status_2;                  // 0x1B
    collision_display_reg_t              collision_display;              // 0x1C
    num_tx_bytes_reg_1_t                 num_tx_bytes_1;                 // 0x1D
    num_tx_bytes_reg_2_t                 num_tx_bytes_2;                 // 0x1E
    nfcip_bit_rate_display_reg_t         nfcip_bit_rate_display;         // 0x1F
    adc_output_reg_t                     adc_output;                     // 0x20
    
    uint8_t                              reserved_21_23[3];              // 0x21 - 0x23 (Reserved)
    
    am_mod_depth_control_reg_t           am_mod_depth_control;           // 0x24
    am_mod_depth_display_reg_t           am_mod_depth_display;           // 0x25
    rfo_am_mod_level_def_reg_t           rfo_am_mod_level_def;           // 0x26
    rfo_normal_level_def_reg_t           rfo_normal_level_def;           // 0x27
    
    uint8_t                              reserved_28;                    // 0x28 (Reserved)
    
    ext_field_detector_thresh_reg_t      ext_field_detector_thresh;      // 0x29
    regulator_voltage_control_reg_t      regulator_voltage_control;      // 0x2A
    reg_timer_display_reg_t              reg_timer_display;              // 0x2B
    rssi_display_reg_t                   rssi_display;                   // 0x2C
    gain_reduction_state_reg_t           gain_reduction_state;           // 0x2D
    
    uint8_t                              reserved_2E_2F[2];              // 0x2E - 0x2F (Reserved)
    
    auxiliary_display_reg_t              auxiliary_display;              // 0x30
    wake_up_timer_control_reg_t          wake_up_timer_control;          // 0x31
    am_measure_config_reg_t              am_measure_config;              // 0x32
    am_measure_ref_reg_t                 am_measure_ref;                 // 0x33
    am_measure_auto_avg_display_reg_t    am_measure_auto_avg_display;    // 0x34
    am_measure_display_reg_t             am_measure_display;             // 0x35
    phase_measure_config_reg_t           phase_measure_config;           // 0x36
    phase_measure_ref_reg_t              phase_measure_ref;              // 0x37
    phase_measure_auto_avg_display_reg_t phase_measure_auto_avg_display; // 0x38
    phase_measure_display_reg_t          phase_measure_display;          // 0x39
    
    uint8_t                              reserved_3A_3E[5];              // 0x3A - 0x3E (Reserved)
    
    ic_identity_reg_t                    ic_identity;                    // 0x3F
} st25r3912_registers;

#endif // ST25R3912_REGISTERS_H