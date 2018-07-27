#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/i2c.h>
#include <esp_err.h>
#include <esp_log.h>
#include <sdkconfig.h>

#include <TRB_AS3935.h>

#define DONTCARE_ZERO   (0)

int32_t
i2c_init()
{
	int32_t r;
	const i2c_port_t i2c_port = I2C_NUM_0;


	i2c_config_t i2c_config;
	i2c_config.mode = I2C_MODE_MASTER;
	i2c_config.sda_io_num = GPIO_SDA;
	i2c_config.scl_io_num = GPIO_SCL;
	i2c_config.sda_pullup_en = GPIO_PULLUP_DISABLE;
	i2c_config.scl_pullup_en = GPIO_PULLUP_DISABLE;
	i2c_config.master.clk_speed = 100000L; // 100KHz

	if ((r = i2c_param_config(i2c_port, &i2c_config)) != 0) {
		ESP_LOGE(__func__, "i2c_param_config(): %d", r);
		goto fail;
	}
	if ((r = i2c_driver_install(
	    i2c_port, i2c_config.mode,
	    DONTCARE_ZERO, DONTCARE_ZERO, DONTCARE_ZERO)) != 0) {
		ESP_LOGE(__func__, "i2c_driver_install(): %d", r);
		goto fail;
	}
	as3935_i2c_set_port(i2c_port);
fail:
	return r;
}

void
app_main(void)
{
	int32_t r;

	as3935_i2c_config_t config;
	config.address = 0x00;
	config.irq_pin = IRQ_PIN;

	ESP_LOGI(__func__, "Initializing I2C bus.");
	if ((r = i2c_init()) != 0) {
		ESP_LOGE(__func__, "i2c_init(): 0x%x", r);
		goto fail;
	}
	ESP_LOGI(__func__, "Initializing as3935 config.");
	if ((r = as3935_init(config)) != 0) {
		ESP_LOGE(__func__, "as3935_init(): 0x%x", r);
		goto fail;
	}
	ESP_LOGI(__func__, "Resetting as3935.");
	if ((r = as3935_reset()) != 0) {
		ESP_LOGE(__func__, "as3935_reset(): 0x%x", r);
		goto fail;
	}
	ESP_LOGI(__func__, "Tuning anttena.");
	if ((r = as3935_tune_anttena(IRQ_PIN, AS3935_FREQ_DIVISION_16)) != 0) {
		ESP_LOGE(__func__, "as3935_tune_anttena(): 0x%x", r);
		goto fail;
	}
fail:
    ESP_LOGI(__func__, "Tests finished.");
    return;
}
