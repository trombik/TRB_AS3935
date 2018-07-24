#if !defined(_TRB_AS3935_ESP_IDF_h)
#define _TRB_AS3935_ESP_IDF_h

#include <stdint.h>
#include <driver/i2c.h>

/*!
 * @brief read a byte from a register
 */
uint32_t
as3935_i2c_read8(const uint8_t addr, const uint8_t reg, uint8_t *value);

/*!
 * @brief write a byte to a register
 */
uint32_t
as3935_i2c_write8(const uint8_t addr, const uint8_t reg, uint8_t value);

/*!
 * @brief set i2c port to use
 */
i2c_port_t
as3935_i2c_set_port(i2c_port_t port);

/*!
 * @brief get i2c port to use
 */
i2c_port_t
as3935_i2c_get_port();

#endif
