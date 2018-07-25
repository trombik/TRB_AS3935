#include <stdint.h>
#include <assert.h>
#include <errno.h>

#if defined(TRB_AS3935_ESP_IDF)
#include <esp_log.h>
#endif

#include "TRB_AS3935.h"

#if defined(TRB_AS3935_ESP_IDF)
#include "sys/esp_idf/i2c.c"
#include "sys/esp_idf/delay.c"
#include "sys/esp_idf/freq_count.c"
#endif

#if defined(__cplusplus)
extern "C" {
#endif

static struct as3935_i2c_config_t config;

int8_t
as3935_init(const struct as3935_i2c_config_t c)
{
	uint8_t r = 0;
	config = c;
	return r;
}

struct as3935_i2c_config_t
as3935_get_config()
{
	return config;
}

int32_t
as3935_reset()
{
	return as3935_i2c_write8(config.address, AS3935_COMMAND_PRESET_DEFAULT);
}

int32_t
as3935_tune_anttena(const uint16_t irq_pin, const uint8_t freq_div)
{
	uint8_t divider;
	uint8_t cap_value, best_cap_value;
	uint32_t target_count;
	int32_t diff, best_diff;
	int32_t r;
	switch (freq_div) {
	case AS3935_FREQ_DIVISION_16:
		divider = 16;
		break;
	case AS3935_FREQ_DIVISION_32:
		divider = 32;
		break;
	case AS3935_FREQ_DIVISION_64:
		divider = 64;
		break;
	case AS3935_FREQ_DIVISION_128:
		divider = 128;
		break;
	default:
		r = EINVAL;
		goto fail;
	}
	assert(divider > 0);
	target_count = 500000 / divider;
	target_count = target_count * AS3935_TUNING_DELAY_MS / 1000;

	if ((r = as3935_set_freq_division(freq_div)) != 0)
		goto fail;

	best_cap_value = 0;
	best_diff = 0xffff;
	for (cap_value = 0; cap_value <= 15; cap_value++) {
		int16_t counter;
		/* set cap */
		if ((r = as3935_set_capacitor(cap_value)) != 0)
			goto fail;
		/* display resonance */
		if ((r = as3935_dsiplay_lco()) != 0)
			goto fail;
		/* wait for LCO to settle */
		as3935_delay_ms(AS3935_TRCOCAL);
		/* count it */
		counter = 0;
		if ((r = as3935_count_on_irq_pin(
		    irq_pin,
		    AS3935_TUNING_DELAY_MS,
		    &counter)) != 0)
			goto fail;
		/* stop LCO */
		if ((r = as3935_stop_lco()) != 0)
			goto fail;
		diff = target_count - counter;
		/* in some frameworks, abs() is a macro */
		if (diff < 0)
			diff *= -1;
		if (diff < best_diff) {
			best_diff = diff;
			best_cap_value = cap_value;
		}
#if defined(TRB_AS3935_ESP_IDF)
        ESP_LOGD(__func__,
                "target_count: %d, cap_value: %d, counter: %d,"
                "diff: %d, best_diff: %d, best_cap_value: %d",
                target_count, cap_value, counter, diff, best_diff,
                best_cap_value);
#endif
	}
	if ((r = as3935_set_capacitor(best_cap_value)) != 0)
	    goto fail;
#if defined(TRB_AS3935_ESP_IDF)
	ESP_LOGD(__func__, "best cap value: %d", best_cap_value);
#endif
fail:
	return r;
}

int32_t
as3935_calibrate_rco()
{
	uint8_t calib_done;
	uint8_t reg_value;
	uint8_t retry;
	uint8_t status;
	uint8_t trco_status, srco_status;
	int32_t r;

	if ((r = as3935_get_register_bits(AS3935_PWD, &reg_value)) != 0)
		goto fail;
	if ((r = as3935_i2c_write8(config.address, AS3935_COMMAND_CALIB_RCO)) !=0)
		goto fail;
	if (reg_value == AS3935_PWD_DOWN) {

		/* when in power down mode, a special procedure is required */
		if ((r = as3935_set_register_bits(AS3935_DISP_SRCO, 1)) != 0)
			goto fail;
		as3935_delay_ms(2);
		if ((r = as3935_set_register_bits(AS3935_DISP_SRCO, 0)) != 0)
			goto fail;
	}
	calib_done = 0;
	for (retry = AS3935_CALIB_RETRY; retry > 0; retry--) {
		if ((r = as3935_get_register_bits(AS3935_TRCO_CALIB_DONE, &trco_status)) != 0)
			goto retry;
		if ((r = as3935_get_register_bits(AS3935_SRCO_CALIB_DONE, &srco_status)) != 0)
			goto retry;
		if (trco_status == AS3935_CALIB_DONE && srco_status == AS3935_CALIB_DONE) {
			calib_done = 1;
			break;
		}
retry:
		as3935_delay_ms(AS3935_CALIB_DELAY_MS);
	}
	if (calib_done != 1) {
		r = ETIMEDOUT;
		goto fail;
	}
	if ((r = as3935_get_register_bits(AS3935_TRCO_CALIB_NOK, &status)) != 0)
		goto fail;
	if (status != AS3935_CALIB_OK) {
		r = ENXIO; // fatal
		goto fail;
	}
	if ((r = as3935_get_register_bits(AS3935_TRCO_CALIB_NOK, &status)) != 0)
		goto fail;
	if (status != AS3935_CALIB_OK) {
		r = ENXIO; // fatal
		goto fail;
	}
fail:
	return r;
}

int32_t
as3935_get_afe_mode(uint8_t *value)
{
	return as3935_get_register_bits(AS3935_AFE_GB, value);
}

int32_t
as3935_set_indoor_mode()
{
	return as3935_set_register_bits(AS3935_AFE_GB, AS3935_INDOOR_MODE);
}

int32_t
as3935_set_outdoor_mode()
{
	return as3935_set_register_bits(AS3935_AFE_GB, AS3935_OUTDOOR_MODE);
}

int32_t
as3935_set_noise_floor_level(const uint8_t value)
{
	return as3935_set_register_bits(AS3935_NF_LEV, value);
}

int32_t
as3935_get_noise_floor_level(uint8_t *value)
{
	return as3935_get_register_bits(AS3935_NF_LEV, value);
}

int32_t
as3935_get_power_mode(uint8_t *value)
{
	return as3935_get_register_bits(AS3935_PWD, value);
}

int32_t
as3935_get_watchdog_threshold(uint8_t *value)
{
	return as3935_get_register_bits(AS3935_WDTH, value);
}

int32_t
as3935_set_watchdog_threshold(uint8_t value)
{
	return as3935_set_register_bits(AS3935_WDTH, value);
}

int32_t
as3935_clear_stats()
{
	int32_t r;

	if ((r = as3935_set_register_bits(AS3935_CL_STAT, 1)) != 0)
		goto fail;
	if ((r = as3935_set_register_bits(AS3935_CL_STAT, 0)) != 0)
		goto fail;
	if ((r = as3935_set_register_bits(AS3935_CL_STAT, 1)) != 0)
		goto fail;
fail:
	return r;
}

int32_t
as3935_set_min_lightning(const uint8_t value)
{
	return as3935_set_register_bits(AS3935_MIN_NUM_LIGH, value);
}

int32_t
as3935_get_min_lightning(uint8_t *value)
{
	return as3935_get_register_bits(AS3935_MIN_NUM_LIGH, value);
}

int32_t
as3935_set_spike_rejection(const uint8_t value)
{
	return as3935_set_register_bits(AS3935_SREJ, value);
}

int32_t
as3935_get_spike_rejection(uint8_t *value)
{
	return as3935_get_register_bits(AS3935_SREJ, value);
}

int32_t
as3935_set_freq_division(const uint8_t value)
{
	return as3935_set_register_bits(AS3935_LCO_FDIV, value);
}

int32_t
as3935_get_freq_division(uint8_t *value)
{
	return as3935_get_register_bits(AS3935_LCO_FDIV, value);
}

int32_t
as3935_get_mask_disturber(uint8_t *value)
{
	return as3935_get_register_bits(AS3935_MASK_DIST, value);
}

int32_t
as3935_enable_mask_disturber()
{
	return as3935_set_register_bits(AS3935_MASK_DIST,
	    AS3935_MASK_DISTUERBER_ENABLED);
}

int32_t
as3935_disable_mask_disturber()
{
	return as3935_set_register_bits(AS3935_MASK_DIST,
	    AS3935_MASK_DISTUERBER_DISABLED);
}

int32_t
as3935_get_interrupt_reason(uint8_t *value)
{
	return as3935_get_register_bits(AS3935_INT, value);
}

int32_t
as3935_get_energy(uint16_t *value)
{
	uint8_t lsb, msb, mmsb;
	int32_t r;

	if ((r = as3935_get_register_bits(AS3935_S_LIG_L, &lsb)) != 0)
		goto fail;
	if ((r = as3935_get_register_bits(AS3935_S_LIG_M, &msb)) != 0)
		goto fail;
	if ((r = as3935_get_register_bits(AS3935_S_LIG_MM, &mmsb)) != 0)
		goto fail;
	*value = (msb << 8) | lsb; // TODO
fail:
	return ENXIO;

}

int32_t
as3935_get_distance(uint8_t *value)
{
	return as3935_get_register_bits(AS3935_DISTANCE, value);
}

int32_t
as3935_dsiplay_lco()
{
	return as3935_set_register_bits(AS3935_DISP_LCO, 1);
}

int32_t
as3935_stop_lco()
{
	return as3935_set_register_bits(AS3935_DISP_LCO, 0);
}

int32_t
as3935_set_capacitor(uint8_t value)
{
	return as3935_set_register_bits(AS3935_TUN_CAP, value);
}

int8_t
bit_shitft_for(uint8_t mask)
{
	int8_t i;
	i = 0;
	for (i = 0; i < 8; i++) {
		if (((~mask >> i) & 1) != 1)
			break;
	}
	return i;
}

int32_t
as3935_get_register_bits(const uint8_t reg, const uint8_t mask, uint8_t *value)
{
	uint8_t reg_value;
	int32_t r;
	r = 0;
	if (mask == 0) {
		r = -1;
		goto fail;
	}
	if ((r = as3935_i2c_read8(config.address, reg, &reg_value)) != 0)
		goto fail;
	reg_value &= mask;
	reg_value >>= (bit_shitft_for(mask));
	*value = reg_value;
fail:
	return r;
}

int32_t
as3935_set_register_bits(const uint8_t reg, const uint8_t mask, const uint8_t value)
{
	uint8_t reg_value;
	int32_t r;
	if ((r = as3935_i2c_read8(config.address, reg, &reg_value)) != 0)
		goto fail;

	/* clear the masked bits, and set the shifted value */
	reg_value &= ~mask;
	reg_value |= value << bit_shitft_for(mask);

	if ((r = as3935_i2c_write8(config.address, reg, reg_value)) != 0)
		goto fail;
fail:
	return r;
}

#if defined(__cplusplus)
}
#endif
