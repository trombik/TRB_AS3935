#define WITH_REPEATED_START true
#define WITHOUT_REPEATED_START false

#include <Arduino.h>
#include <brzo_i2c.h>
#include <errno.h>

#if defined(__cplusplus)
extern "C" {
#endif

static uint16_t scl_freq = 100; // in KHz

uint16_t
as3935_brzo_get_scl_freq()
{
	return scl_freq;
}

uint16_t
as3935_brzo_set_scl_freq(const uint16_t freq)
{
	scl_freq = freq;
	return scl_freq;
}

int32_t
as3935_i2c_read8(const uint8_t addr, const uint8_t reg, uint8_t *value)
{
	int32_t r = 0;
	uint8_t reg_addr_copy;

	/* avoid const violation */
	reg_addr_copy = addr;

	/* start transaction */
	brzo_i2c_start_transaction(addr, as3935_brzo_get_scl_freq());
	/* write register address with repeated start */
	brzo_i2c_write(&reg_addr_copy, 1, WITH_REPEATED_START);
	/* read the values */
	brzo_i2c_read(value, 1, WITHOUT_REPEATED_START);
	/* end the transaction */
	r = brzo_i2c_end_transaction();
	return r;
}

int32_t
as3935_i2c_write8(const uint8_t addr, const uint8_t reg, uint8_t value)
{
	int32_t r = 0;
	uint8_t buffer[2];

	buffer[0] = reg;
	buffer[1] = value;
	/* start transaction */
	brzo_i2c_start_transaction(addr, as3935_brzo_get_scl_freq());
	/* write register address + the value */
	brzo_i2c_write(buffer, 2, WITHOUT_REPEATED_START);
	/* end end transaction */
	if ((r = brzo_i2c_end_transaction()) != 0) {
		goto fail;
	}
	as3935_delay_ms(10);
fail:
	return r;
}

#if defined(__cplusplus)
}
#endif
