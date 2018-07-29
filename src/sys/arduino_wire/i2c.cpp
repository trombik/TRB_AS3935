#if defined(TRB_AS3935_ARDUINO_WIRE)

#include <Arduino.h>
#include <Wire.h>
#include <errno.h>

#define WITH_REPEATED_START	false

int32_t
as3935_i2c_read8(const uint8_t addr, const uint8_t reg, uint8_t *value)
{
	int32_t r = 0;
	const static uint8_t WITH_STOP_BIT = 1;
	uint8_t length = 1;

	/* start transaction */
	Wire.beginTransmission(addr);
	/* write register address */
	if ((r = Wire.write(reg)) != 1) {
		r = EIO;
		goto fail;
	}
	/* read the values with repeated start, and end the transaction with stop
	* bit */
	if ((r = Wire.endTransmission(WITH_REPEATED_START)) != 0)
		goto fail;
	if (Wire.requestFrom(addr, length, WITH_STOP_BIT) != 1) {
		r = EIO;
		goto fail;
	}
	*value = Wire.read();
fail:
	return r;
}

int32_t
as3935_i2c_write8(const uint8_t addr, const uint8_t reg, const uint8_t value)
{
	int32_t r = 0;
	uint8_t length = 1;

	/* start transaction */
	Wire.beginTransmission(addr);
	/* write register address */
	if ((r = Wire.write(reg)) != 1) {
		r = EIO;
		goto fail;
	}
	/* write values */
	if ((r = Wire.write(&value, length)) != 1) {
		r = EIO;
		goto fail;
	}
	/* end end transaction */
	r = Wire.endTransmission();
fail:
	return r;
}

#endif // TRB_AS3935_ARDUINO_WIRE
