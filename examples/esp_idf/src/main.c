#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/i2c.h>
#include <esp_err.h>
#include <esp_log.h>
#include "driver/gpio.h"
#include <sdkconfig.h>

#include <TRB_AS3935.h>

#define DONTCARE_ZERO   (0)
#define ESP_INTR_FLAG_DEFAULT 0

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
task_handle_irq(void * pvParameters)
{
	int32_t r;
	as3935_i2c_config_t config;
	config.address = I2C_ADDR;
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

	ESP_LOGI(__func__, "Initialization finished, looping");
	while (1) {
		uint8_t distance, reason;

		/* suspend until an event occurs */
		ESP_LOGI(__func__, "suspending...");
		vTaskSuspend(NULL);
		ESP_LOGI(__func__, "Waked up");

		if ((r = as3935_get_interrupt_reason(&reason)) != 0) {
			ESP_LOGE(__func__, "as3935_get_interrupt_reason(): 0x%x", r);
			continue;
		}
		switch (reason) {
		case AS3935_INT_NH:
			ESP_LOGW(__func__, "Noise level too high");
			continue;
		case AS3935_INT_D:
			ESP_LOGW(__func__, "Disturber detected");
			continue;
		case AS3935_INT_L:
			break;
		default:
			ESP_LOGE(__func__, "Unknown reason 0x%x", reason);
			continue;
		}

		if ((r = as3935_get_distance(&distance)) != 0) {
			ESP_LOGE(__func__, "as3935_get_distance(): 0x%x", r);
			continue;
		}
		if (distance == 0b111111) {
			ESP_LOGI(__func__, "distance: Out of Range");
		} else {
			ESP_LOGI(__func__, "distance: %dkm", distance);
		}
	}
fail:
	ESP_LOGI(__func__, "Tests failed.");
	vTaskDelete(NULL);
}

static void IRAM_ATTR
gpio_isr_handler(void *arg)
{
	TaskHandle_t task_h = (TaskHandle_t) arg;
	xTaskResumeFromISR(task_h);
}

void
app_main(void)
{
	TaskHandle_t task_h;
	gpio_config_t io_conf;

    vTaskDelay(10000 / portTICK_PERIOD_MS);

	io_conf.intr_type = GPIO_INTR_POSEDGE; // rising edge
	io_conf.mode = GPIO_MODE_INPUT;
	io_conf.pin_bit_mask = (1 << IRQ_PIN);
	io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
	io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
	gpio_config(&io_conf);

	xTaskCreate(
	    task_handle_irq,
	    "handle_irq",
	    configMINIMAL_STACK_SIZE * 3,
	    NULL,
	    1,
	    &task_h
	);
	configASSERT(task_h);
	gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
	gpio_isr_handler_add(IRQ_PIN, gpio_isr_handler, (void*) task_h);
}
