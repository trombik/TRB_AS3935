#include <stdint.h>
#include <esp_err.h>

/*!
 * Count rising edges on IRQ pin
 *
 * @param[in] irq_pin : Pin number that is connected to IRQ pin of AS3935
 * @param[in] duration_ms : Duration in milli sec to count
 * @param[out] counter : Variable to write the result to
 */
esp_err_t
as3935_count_on_irq_pin(const uint16_t irq_pin, const uint8_t duration_ms, int16_t *counter);
