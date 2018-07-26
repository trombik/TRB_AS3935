#if defined(TRB_AS3935_ESP_IDF)

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

void
as3935_delay_ms(const uint16_t milli)
{
	vTaskDelay(milli / portTICK_PERIOD_MS);
}

#endif // TRB_AS3935_ESP_IDF
