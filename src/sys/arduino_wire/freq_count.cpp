#if defined(TRB_AS3935_ARDUINO_WIRE)

#include <Arduino.h>

int32_t
as3935_count_on_irq_pin(const uint16_t irq_pin, const uint8_t duration_ms, int16_t *counter)
{
	uint32_t sheduled_end;
	uint8_t current_level, previous_level;

	*counter = 0;
	current_level = LOW;
	pinMode(irq_pin, INPUT);
	sheduled_end = millis() + duration_ms;
	previous_level = digitalRead(irq_pin);
	while ((sheduled_end - millis()) > 0) {
		current_level = digitalRead(irq_pin);
		if (previous_level != current_level)
			*counter += 1;
		previous_level = current_level;
	}
	return 0;
}

#endif // TRB_AS3935_ARDUINO_WIRE
