#if !defined(_TRB_AS3935_ESP_IDF_h)
#define _TRB_AS3935_ESP_IDF_h

#include <driver/i2c.h>


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
