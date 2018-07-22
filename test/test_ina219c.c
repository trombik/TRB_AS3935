#include <driver/i2c.h>
#include <esp_err.h>
#include "unity.h"
#include <esp_log.h>

#include "TRB_AS3935.h"

static i2c_port_t i2c_port = I2C_NUM_1;

static char component[] = "[TRB_AS3935]";

esp_err_t
i2c_init()
{
	i2c_config_t i2c_config;

	i2c_config.mode = I2C_MODE_MASTER;
	i2c_config.sda_io_num = GPIO_SDA;
	i2c_config.scl_io_num = GPIO_SCL;
	i2c_config.sda_pullup_en = GPIO_PULLUP_DISABLE;
	i2c_config.scl_pullup_en = GPIO_PULLUP_DISABLE;
	i2c_config.master.clk_speed = 400000L; // 400KHz

	TEST_ASSERT_EQUAL_INT8(0, i2c_param_config(i2c_port, &i2c_config));
	TEST_ASSERT_EQUAL_INT8(0, i2c_driver_install(i2c_port, i2c_config.mode, 0, 0, 0));
	TEST_ASSERT_EQUAL(I2C_NUM_1, ina219_set_i2c_port(i2c_port));
	return 0;
}

TEST_CASE("ina219_create", component)
{
	dev = ina219_create(i2c_address);
	TEST_ASSERT_EQUAL_UINT8(0, i2c_init())
}
