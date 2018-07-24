#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "delay.h"

void
as3935_delay_ms(const uint16_t milli)
{
	vTaskDelay(milli / portTICK_PERIOD_MS);
}
