#if defined(__cplusplus)
extern "C" {
#endif

uint16_t
as3935_brzo_get_scl_freq();

uint16_t
as3935_brzo_set_scl_freq(const uint16_t freq);

int32_t
as3935_i2c_read8(const uint8_t addr, const uint8_t reg, uint8_t *value);

int32_t
as3935_i2c_write8(const uint8_t addr, const uint8_t reg, uint8_t value);

#if defined(__cplusplus)
}
#endif
