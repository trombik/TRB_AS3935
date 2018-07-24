#include <driver/i2c.h>
#include <esp_err.h>
#include <esp_log.h>
#include <stdint.h>
#include "unity.h"

#include "TRB_AS3935.h"

static i2c_port_t i2c_port = I2C_NUM_1;
static char component[] = "[TRB_AS3935]";
uint8_t reg_value;
uint32_t r;
struct as3935_i2c_config_t config = {
	0x00, // I2C address
	IRQ_PIN
};

uint32_t
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
	TEST_ASSERT_EQUAL(I2C_NUM_1, as3935_i2c_set_port(i2c_port));
	return 0;
}

TEST_CASE("bit_shitft_for", component)
{
	TEST_ASSERT_EQUAL_UINT8(0, bit_shitft_for(1 << 0));
	TEST_ASSERT_EQUAL_UINT8(1, bit_shitft_for(1 << 1));
	TEST_ASSERT_EQUAL_UINT8(1, bit_shitft_for(1 << 2 | 1 << 1));
	TEST_ASSERT_EQUAL_UINT8(2, bit_shitft_for(1 << 3 | 1 << 2));
	TEST_ASSERT_EQUAL_UINT8(7, bit_shitft_for(1 << 7));
}

TEST_CASE("as3935_i2c_set_port", component)
{
	TEST_ASSERT_EQUAL_UINT8(0, i2c_init());
	TEST_ASSERT(as3935_i2c_get_port() == I2C_NUM_1);
	TEST_ASSERT(as3935_i2c_set_port(I2C_NUM_0) == I2C_NUM_0);
	TEST_ASSERT(as3935_i2c_get_port() == I2C_NUM_0);
	TEST_ASSERT_EQUAL_UINT32(0, i2c_driver_delete(i2c_port));
}

TEST_CASE("as3935_get_config", component)
{
	TEST_ASSERT_EQUAL_UINT8(0, as3935_init(config));
	config = as3935_get_config();
	TEST_ASSERT_EQUAL_UINT8(0, config.address);
}

TEST_CASE("i2c_init", component)
{
	TEST_ASSERT_EQUAL_UINT8(0, i2c_init());
	TEST_ASSERT_EQUAL_UINT32(0, i2c_driver_delete(i2c_port));
}

TEST_CASE("as3935_reset", component)
{
	TEST_ASSERT_EQUAL_UINT8(0, i2c_init());
	TEST_ASSERT_EQUAL_INT32(0, as3935_init(config));
	r = as3935_reset();
	TEST_ASSERT_EQUAL_INT32_MESSAGE(0, r, esp_err_to_name(r));
	TEST_ASSERT_EQUAL_UINT32(0, i2c_driver_delete(i2c_port));
}

TEST_CASE("as3935_i2c_read8", component)
{
	TEST_ASSERT_EQUAL_UINT8(0, i2c_init());
	TEST_ASSERT_EQUAL_INT32(0, as3935_init(config));
	r = as3935_i2c_read8(config.address, 0x00, &reg_value);
	TEST_ASSERT_EQUAL_UINT32_MESSAGE(0, r, esp_err_to_name(r));
	TEST_ASSERT_EQUAL_UINT8(0b0100100, reg_value);
	TEST_ASSERT_EQUAL_UINT32(0, i2c_driver_delete(i2c_port));
}

TEST_CASE("as3935_get_register_bits", component)
{
	TEST_ASSERT_EQUAL_UINT8(0, i2c_init());
	TEST_ASSERT_EQUAL_UINT8(0, as3935_init(config));
	TEST_ASSERT_EQUAL_INT32(0, as3935_reset());
	TEST_ASSERT_EQUAL_INT32(0, as3935_get_register_bits(AS3935_AFE_GB, &reg_value));
	TEST_ASSERT_EQUAL_UINT8(AS3935_INDOOR_MODE, reg_value);
	TEST_ASSERT_EQUAL_INT32(0, i2c_driver_delete(i2c_port));
}

TEST_CASE("as3935_set_outdoor_mode", component)
{
	TEST_ASSERT_EQUAL_UINT8(0, i2c_init());
	TEST_ASSERT_EQUAL_INT32(0, as3935_init(config));
	TEST_ASSERT_EQUAL_INT32(0, as3935_reset());
	TEST_ASSERT_EQUAL_INT32(0, as3935_get_afe_mode(&reg_value));
	TEST_ASSERT_EQUAL_UINT8(AS3935_INDOOR_MODE, reg_value);
	TEST_ASSERT_EQUAL_INT32(0, as3935_set_outdoor_mode());
	TEST_ASSERT_EQUAL_INT32(0, as3935_get_afe_mode(&reg_value));
	TEST_ASSERT_EQUAL_UINT8(AS3935_OUTDOOR_MODE, reg_value);
	TEST_ASSERT_EQUAL_INT32(0, i2c_driver_delete(i2c_port));
}

TEST_CASE("tuning", component)
{
	TEST_ASSERT_EQUAL_UINT8(0, i2c_init());
	TEST_ASSERT_EQUAL_UINT8(0, as3935_init(config));
	TEST_ASSERT_EQUAL_INT32(0, as3935_reset());
	TEST_ASSERT_EQUAL_INT32(0, as3935_tune_anttena(IRQ_PIN, AS3935_FREQ_DIVISION_16));
	TEST_ASSERT_EQUAL_INT32(0, i2c_driver_delete(i2c_port));
}
