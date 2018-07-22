#include <stdio.h>
#include "unity.h"
#include "unity_config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "tcpip_adapter.h"

#include "TRB_AS3935.h"

void
unityTask(void *pvParameters) {
	vTaskDelay(1000 / portTICK_PERIOD_MS);
	unity_run_menu();
}

void
app_main() {
	/* TCP/IP adapter is initialized here because it leaks memory so the
	 * initialization in test cases would make the test fail because of
	 * leak. */
	tcpip_adapter_init();
	/* Note: if unpinning this task, change the way run times are calculated
	 * in unity_platform */
	xTaskCreatePinnedToCore(unityTask, "unityTask", UNITY_FREERTOS_STACK_SIZE, NULL,
	    UNITY_FREERTOS_PRIORITY, NULL, UNITY_FREERTOS_CPU);
}
