#if !defined(_TRB_AS3935_h)
#define _TRB_AS3935_h

#include <stdint.h>

#if defined(TRB_AS3935_ESP_IDF)
#include "sys/esp_idf/i2c.h"
#endif

#if defined(__cplusplus)
extern "C" {
#endif

#define AS3935_AFE_GB		0x00, 0x3E
#define AS3935_PWD		0x00, 0x01
#define AS3935_NF_LEV		0x01, 0x70
#define AS3935_WDTH		0x01, 0x0F
#define AS3935_CL_STAT		0x02, 0x40
#define AS3935_MIN_NUM_LIGH	0x02, 0x30
#define AS3935_SREJ		0x02, 0x0F
#define AS3935_LCO_FDIV		0x03, 0xC0
#define AS3935_MASK_DIST	0x03, 0x20
#define AS3935_INT		0x03, 0x0F
#define AS3935_S_LIG_L		0x04, 0xFF
#define AS3935_S_LIG_M		0x05, 0xFF
#define AS3935_S_LIG_MM		0x06, 0x1F
#define AS3935_DISTANCE		0x07, 0x3F
#define AS3935_DISP_LCO		0x08, 0x80
#define AS3935_DISP_SRCO	0x08, 0x40
#define AS3935_DISP_TRCO	0x08, 0x20
#define AS3935_TUN_CAP		0x08, 0x0F
#define AS3935_TRCO_CALIB_DONE	0x3A, (1 << 7)
#define AS3935_TRCO_CALIB_NOK	0x3A, (1 << 6)
#define AS3935_SRCO_CALIB_DONE	0x3B, (1 << 7)
#define AS3935_SRCO_CALIB_NOK	0x3B, (1 << 6)
#define AS3935_ENERGY_1		0x04, 0xFF
#define AS3935_ENERGY_2		0x05, 0xFF
#define AS3935_ENERGY_3		0x06, 0x1F

#define AS3935_PWD_DOWN	(1)
#define AS3935_PWD_UP	(0)

#define AS3935_FREQ_DIVISION_16		(0b00)
#define AS3935_FREQ_DIVISION_32		(0b01)
#define AS3935_FREQ_DIVISION_64		(0b10)
#define AS3935_FREQ_DIVISION_128	(0b11)

#define AS3935_MASK_DISTUERBER_ENABLED	(1)
#define AS3935_MASK_DISTUERBER_DISABLED	(0)

#define AS3935_CALIB_DONE	(1)
#define AS3935_CALIB_OK		(0)

#define AS3935_INT_NH	(0b0001) /**<  Noise level too high */
#define AS3935_INT_D	(0b0100) /**< Disturber detected */
#define AS3935_INT_L	(0b1000) /**<  Lightning interrupt */

/* The calibration of the RC oscillators starts after the LCO settles */
#define AS3935_TRCOCAL	(2)

#if !defined(AS3935_CALIB_RETRY)
#define AS3935_CALIB_RETRY	(5)
#endif

#if !defined(AS3935_CALIB_DELAY_MS)
#define AS3935_CALIB_DELAY_MS	(10)
#endif

#if !defined(AS3935_TUNING_DELAY_MS)
#define AS3935_TUNING_DELAY_MS	(100)
#endif

typedef struct {
	uint8_t address;
	uint16_t irq_pin;
} as3935_i2c_config_t;

/*!
 * @brief Direct commands described in page 28.
 */
#define AS3935_COMMAND_PRESET_DEFAULT	0x3C, 0x96
#define AS3935_COMMAND_CALIB_RCO	0x3D, 0x96

/*!
 * @brief Analog Front-End mode.
 */
#define AS3935_INDOOR_MODE	(0b10010) /**< Indoor */
#define AS3935_OUTDOOR_MODE	(0b01110) /**< Outdoor */

/*!
 * Intialize the configuration.
 *
 * @param[in] as3935_i2c_config_t
 */
int8_t
as3935_init(const as3935_i2c_config_t c);

/*!
 * Get current configuration.
 *
 * @returns as3935_i2c_config_t
 */
as3935_i2c_config_t
as3935_get_config();

/*!
 * Reset the device to default.
 *
 * @returns Zero when successful, error code when failed
 */
int32_t
as3935_reset();

/*!
 * Tune the anttena by choosing capacitor value.
 *
 * @param[in] IRQ pin number
 * @param[in] freq_div : Frequency Division Ratio
 * @returns Zero when successful, error code when failed
 */
int32_t
as3935_tune_anttena(const uint16_t irq_pin, const uint8_t freq_div);

/*!
 * Calibrate RCO
 *
 * @returns Zero when successful, error code when failed
 */
int32_t
as3935_calibrate_rco();

/*!
 * Get AFE mode, AS3935_OUTDOOR_MODE or AS3935_INDOOR_MODE.
 *
 * @param[out] Pointer of variable to write the value to
 * @returns Zero when successful, error code when failed
 */
int32_t
as3935_get_afe_mode(uint8_t *value);

/*!
 * Set AFE to indoor mode.
 *
 * @returns Zero when successful, error code when failed
 */
int32_t
as3935_set_indoor_mode();

/*!
 * Set AFE to outdoor mode.
 *
 * @returns Zero when successful, error code when failed
 */
int32_t
as3935_set_outdoor_mode();

/*!
 * Set noise floor level.
 *
 * @param[in] Noise floor level
 */
int32_t
as3935_set_noise_floor_level(const uint8_t value);

/*!
 * Get noise floow level.
 *
 * @param[out] Pointer to valiable to write the result to.
 */
int32_t
as3935_get_noise_floor_level(uint8_t *value);

/*!
 * Get the current power mode.
 *
 * @param[out] Pointer to valiable to write the result to.
 * @returns Zero when successful, error code when failed
 */
int32_t
as3935_get_power_mode(uint8_t *value);

/*! Get the current watchdog threshold.
 *
 * @param[out] Pointer to valiable to write the result to.
 * @returns Zero when successful, error code when failed
 */
int32_t
as3935_get_watchdog_threshold(uint8_t *value);

/*!
 * Set watchdog threshold.
 *
 * @param[in] watchdog threshold
 * @returns Zero when successful, error code when failed
 */
int32_t
as3935_set_watchdog_threshold(uint8_t value);

/*!
 * Clear statistic data in memory
 *
 * @returns Zero when successful, error code when failed
 */
int32_t
as3935_clear_stats();

/*!
 * Set minimum number of lightnings
 *
 * @param number of lightnings
 * @returns Zero when successful, error code when failed
 */
int32_t
as3935_set_min_lightning(const uint8_t value);

/*!
 * Get the current minimum number of lightnings
 *
 * @param[out] Pointer of variable to write the value
 * @returns Zero when successful, error code when failed
 */
int32_t
as3935_get_min_lightning(uint8_t *value);

/*!
 * Set spike rejection setting. See page 31, Signal Validation.
 *
 * @param[in] signal rejection value
 * @returns Zero when successful, error code when failed
 */
int32_t
as3935_set_spike_rejection(const uint8_t value);

/*!
 * Get current spike rejection value.
 *
 * @param[out] Pointer of variable to write the value
 * @returns Zero when successful, error code when failed
 */
int32_t
as3935_get_spike_rejection(uint8_t *value);

/*!
 * Set Frequency Division Ratio for the Antenna Tuning.
 *
 * @param[in] Value to set
 * @returns Zero when successful, error code when failed
 */
int32_t
as3935_set_freq_division(const uint8_t value);

/*!
 * Get Frequency Division Ratio for the Antenna Tuning
 *
 * @param[out] Pointer of variable to write the value
 * @returns Zero when successful, error code when failed
 */
int32_t
as3935_get_freq_division(uint8_t *value);

/*!
 * Get current value of mask disturber.
 *
 * @param[out] Pointer of variable to write the value
 * @returns Zero when successful, error code when failed
 */
int32_t
as3935_get_mask_disturber(uint8_t *value);

/*!
 * Enable mask disturber
 *
 * @returns Zero when successful, error code when failed
 */
int32_t
as3935_enable_mask_disturber();

/*!
 * Disable mask disturber.
 *
 * @returns Zero when successful, error code when failed
 */
int32_t
as3935_disable_mask_disturber();

/*!
 * Get the cause of interrupt.
 *
 * @param[out] Pointer of variable to write the value
 * @returns Zero when successful, error code when failed
 */
int32_t
as3935_get_interrupt_reason(uint8_t *value);

/*!
 * Get estimated energy of lightning (not implemented)
 *
 * @param[out] Pointer of variable to write the value
 * @returns Zero when successful, error code when failed
 */
int32_t
as3935_get_energy(uint16_t *value);

/*!
 * Get estimated distance to the storm
 *
 * @param[out] Pointer of variable to write the value
 * @returns Zero when successful, error code when failed
 */
int32_t
as3935_get_distance(uint8_t *value);

/*!
 * Display LCO on IRQ pin
 *
 * @returns Zero when successful, error code when failed
 */
int32_t
as3935_dsiplay_lco();

/*
 * Stop displaying LCO on IRQ pin
 *
 * @returns Zero when successful, error code when failed
 */
int32_t
as3935_stop_lco();

/*!
 * Set tuning capacitor
 */
int32_t
as3935_set_capacitor(uint8_t value);

/*!
 * Get tuning capacitor value
 */
int32_t
as3935_get_capacitor(uint8_t *value);

/*!
 * Return register bits masked by the given mask. The mask must not be zero. The
 * value is shifted so that the returned value equals to the bit value.
 */
int32_t
as3935_get_register_bits(const uint8_t reg, const uint8_t mask, uint8_t *value);

/*!
 * Return the number of zero in lower side of the given mask.  Given the mask is
 * 0b00001100, returns 2. The mask must not be zero.
 */
int8_t
bit_shitft_for(uint8_t mask);

int32_t
as3935_set_register_bits(const uint8_t reg, const uint8_t mask, const uint8_t value);

/*!
 * Functions each framework must implement
 */

/*!
 * Delay N milli seconds
 *
 * @param[in] milli : millisec to wait
 */
void
as3935_delay_ms(const uint16_t milli);

/*!
 * @brief read a byte from a register
 *
 * @param[in] addr : I2C address
 * @param[in] reg : Register address
 * @param[out] value : value to write the register value to
 */
int32_t
as3935_i2c_read8(const uint8_t addr, const uint8_t reg, uint8_t *value);

/*!
 * @brief write a byte to a register
 *
 * @param[in] addr : I2C address
 * @param[in] reg : Register address
 * @param[out] value : value to write to the register
 */

int32_t
as3935_i2c_write8(const uint8_t addr, const uint8_t reg, uint8_t value);

/*!
 * Count rising edges on IRQ pin
 *
 * @param[in] irq_pin : Pin number that is connected to IRQ pin of AS3935
 * @param[in] duration_ms : Duration in milli sec to count
 * @param[out] counter : Variable to write the result to
 */
int32_t
as3935_count_on_irq_pin(const uint16_t irq_pin, const uint8_t duration_ms, int16_t *counter);

#if defined(__cplusplus)
}
#endif

#endif
