#if defined(TRB_AS3935_ESP_IDF)

#include <driver/i2c.h>
#include <esp_log.h>
#include "sys/esp_idf/i2c.h"

#define ACK_CHECK_ENABLE 0x01

static i2c_port_t i2c_port = I2C_NUM_0;

i2c_port_t
as3935_i2c_set_port(i2c_port_t port)
{
	i2c_port = port;
	return i2c_port;
}

i2c_port_t
as3935_i2c_get_port()
{
	return i2c_port;
}

int32_t
as3935_i2c_read8(const uint8_t addr, const uint8_t reg, uint8_t *value)
{
	int32_t r = 0;
	i2c_cmd_handle_t command;
	uint8_t retry = 3;

	command = i2c_cmd_link_create();
	do {
		if ((r = i2c_master_start(command)) != ESP_OK)
			goto fail;
		if ((r = i2c_master_write_byte(command, (addr << 1) | I2C_MASTER_WRITE, ACK_CHECK_ENABLE)) != ESP_OK)
			goto fail;
		if ((r = i2c_master_write_byte(command, reg, ACK_CHECK_ENABLE)) != ESP_OK)
			goto fail;
		if ((r = i2c_master_start(command)) != ESP_OK)
			goto fail;
		if ((r = i2c_master_write_byte(command, (addr << 1) | I2C_MASTER_READ, ACK_CHECK_ENABLE)) != ESP_OK)
			goto fail;
		if ((r = i2c_master_read(command, value, 1, I2C_MASTER_NACK)) != ESP_OK)
			goto fail;
		i2c_master_stop(command);

		r = i2c_master_cmd_begin(i2c_port, command, 10 / portTICK_PERIOD_MS);
		retry--;
	} while (retry > 0 && r != ESP_OK);
fail:
	if (r != 0)
#if defined(HAVE_ESP_ERR_TO_NAME)
		ESP_LOGE(__func__, "i2c_master_* failed with: %s", esp_err_to_name(r));
#else
		ESP_LOGE(__func__, "i2c_master_* failed with: %d", r);
#endif

	i2c_cmd_link_delete(command);
	return r;
}

int32_t
as3935_i2c_write8(const uint8_t addr, const uint8_t reg, uint8_t value)
{
	int32_t r = 0;
	i2c_cmd_handle_t command;
	uint8_t retry = 3;

	command = i2c_cmd_link_create();
	do {
		if ((r = i2c_master_start(command)) != ESP_OK)
			goto fail;
		if ((r = i2c_master_write_byte(command, (addr << 1) | I2C_MASTER_WRITE, ACK_CHECK_ENABLE)) != ESP_OK)
			goto fail;
		if ((r = i2c_master_write_byte(command, reg, ACK_CHECK_ENABLE)) != ESP_OK)
			goto fail;
		if ((r = i2c_master_write(command, &value, 1, ACK_CHECK_ENABLE)) != ESP_OK)
			goto fail;
		if ((r = i2c_master_stop(command)) != ESP_OK)
			goto fail;
		r = i2c_master_cmd_begin(i2c_port, command, 10 / portTICK_PERIOD_MS);
		if (r != 0)
#if defined(HAVE_ESP_ERR_TO_NAME)
			ESP_LOGE(__func__, "i2c_master_* failed with: %s", esp_err_to_name(r));
#else
			ESP_LOGE(__func__, "i2c_master_* failed with: %d", r);
#endif
		retry--;
	} while (retry > 0 && r != ESP_OK);
fail:
	i2c_cmd_link_delete(command);
	return r;
}

#endif // TRB_AS3935_ESP_IDF
