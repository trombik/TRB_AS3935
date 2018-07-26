#if defined(TRB_AS3935_ESP_IDF)

#include <driver/pcnt.h>
#include <esp_err.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>

esp_err_t
as3935_count_on_irq_pin(const uint16_t irq_pin, const uint8_t duration_ms, int16_t *counter)
{
	uint32_t r;
	pcnt_config_t pcnt_config = {
		.pulse_gpio_num = irq_pin,
		.ctrl_gpio_num = PCNT_PIN_NOT_USED,
		.lctrl_mode = PCNT_MODE_KEEP,
		.hctrl_mode = PCNT_MODE_KEEP,
		.pos_mode = PCNT_COUNT_INC,
		.neg_mode = PCNT_COUNT_DIS,
		.counter_h_lim = 0xffff,
		.counter_l_lim = 0,
		.unit = PCNT_UNIT_0,
		.channel = PCNT_CHANNEL_0
	};
	/* PCNT starts */
	if ((r = pcnt_unit_config(&pcnt_config)) != 0)
		goto fail;
	/* reset the PCNT */
	if ((r = pcnt_counter_pause(PCNT_UNIT_0)) != 0)
		goto counter_fail;
	if ((r = pcnt_counter_clear(PCNT_UNIT_0)) != 0)
		goto counter_fail;
	/* count it */
	if ((r = pcnt_counter_resume(PCNT_UNIT_0)) != 0)
		goto counter_fail;
	vTaskDelay(duration_ms / portTICK_PERIOD_MS);
	/* stop counting it */
	if ((r = pcnt_get_counter_value(PCNT_UNIT_0, counter)) != 0)
		goto counter_fail;
counter_fail:
	if (pcnt_counter_pause(PCNT_UNIT_0) != 0)
		ESP_LOGE(__func__, "pcnt_counter_pause() failed");
	if (pcnt_counter_clear(PCNT_UNIT_0) != 0)
		ESP_LOGE(__func__, "pcnt_counter_clear() failed");
fail:
	if (r != 0)
#if defined(HAVE_ESP_ERR_TO_NAME)
		ESP_LOGE(__func__, "PCNT failed with: %s", esp_err_to_name(r));
#else
		ESP_LOGE(__func__, "PCNT failed with: %d", r);
#endif
	return r;
}

#endif // TRB_AS3935_ESP_IDF
