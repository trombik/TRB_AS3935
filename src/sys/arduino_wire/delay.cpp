#if defined(TRB_AS3935_ARDUINO_WIRE)

#include <Arduino.h>

void
as3935_delay_ms(const uint16_t milli)
{
	uint32_t now = millis();
	/* do NOOP instead of delay(), which does not block */
	while (millis() < now + milli) {
#if defined(ESP8266) || defined(ESP32)
		/* avoid watchdog timeout */
		yield();
#endif
	}
}

#endif // TRB_AS3935_ARDUINO_WIRE
