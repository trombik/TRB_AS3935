#include <Arduino.h>
#include <Wire.h>
#include <TRB_AS3935.h>
#include <stdint.h>

void
halt()
{
	Serial.println(F("Halted. Please reset"));
	while(1) {
#if defined(ESP32) || defined(ESP8266)
		yield();
#endif
	}
}

void
setup()
{
	uint8_t reg_value, cap_value;
	int32_t r;
	as3935_i2c_config_t config;

	Serial.begin(115200);
	delay(1000);
	Serial.println(__FILE__);

	Serial.print(F("Initializing I2C... "));
	pinMode(GPIO_SCL, INPUT_PULLUP);
	pinMode(GPIO_SDA, INPUT_PULLUP);
#if defined(ESP32) || defined(ESP8266)
	Wire.begin(GPIO_SDA, GPIO_SCL);
#else
	Wire.begin();
#endif
	Serial.println(F("Initialized."));

	config.address = 0x00;
	config.irq_pin = IRQ_PIN;
	Serial.print(F("Initializing AS3935... "));
	as3935_init(config);
	Serial.println(F("Initialized."));

	Serial.print(F("Reading 0x00 register... "));
	if ((r = as3935_i2c_read8(config.address, 0x00, &reg_value)) != 0) {
		Serial.print(F("Failed to as3935_i2c_read8(): "));
		Serial.println(r);
		halt();
	}
	if (reg_value != 0b0100100) {
		Serial.print(F("wrong default value: 0b"));
		Serial.println(r, BIN);
		halt();
	}
	Serial.println(F("Known default value (0b0100100) found"));

	Serial.print(F("Resetting the device... "));
	if ((r = as3935_reset()) != 0) {
		Serial.print(F("as3935_reset() failed: "));
		Serial.println(r);
		halt();
	}
	Serial.println(F("Done."));

	Serial.print(F("Reading AS3935_AFE_GB... "));
	if ((r = as3935_get_register_bits(AS3935_AFE_GB, &reg_value)) != 0) {
		Serial.print(F("as3935_get_register_bits() failed: "));
		Serial.println(r);
		halt();
	}
	Serial.println(F("Default value (AS3935_INDOOR_MODE) of AS3935_AFE_GB found."));

	Serial.print(F("Setting to outdoor mode... "));
	if ((r = as3935_set_outdoor_mode()) != 0) {
		Serial.print(F("as3935_set_outdoor_mode() failed: "));
		Serial.println(r);
		halt();
	}
	if ((r = as3935_get_afe_mode(&reg_value)) != 0) {
		Serial.print(F("as3935_get_afe_mode() failed: "));
		Serial.println(r);
		halt();
	}
	if (reg_value != AS3935_OUTDOOR_MODE) {
		Serial.print(F("AFE mode is not AS3935_OUTDOOR_MODE: 0b"));
		Serial.println(reg_value, BIN);
	}
	Serial.println(F("AFE mode is AS3935_OUTDOOR_MODE."));

	Serial.print(F("Tuning anttena... "));
	if ((r = as3935_tune_anttena(IRQ_PIN, AS3935_FREQ_DIVISION_16)) != 0) {
		Serial.print(F("as3935_tune_anttena() failed: "));
		Serial.println(r);
		halt();
	}
	Serial.println(F("Done."));

	Serial.print(F("Reading tuned capacitor value... "));
	if ((r = as3935_get_capacitor(&cap_value)) != 0) {
		Serial.print(F("as3935_get_capacitor() failed: "));
		Serial.println(r);
		halt();
	}
	Serial.print(F("Capacitor value: 0b"));
	Serial.println(cap_value, BIN);

	Serial.println(F("Test successfully finished."));
	as3935_reset();
	halt();
}

void
loop()
{
	delay(10);
}
