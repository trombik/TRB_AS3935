#if !defined(TRB_AS3935_cpp)
#define TRB_AS3935_cpp

#include "TRB_AS3935.h"
#include "TRB_AS3935_common.c"

#if defined(TRB_AS3935_ARDUINO_WIRE)
#include "sys/arduino_wire/delay.cpp"
#include "sys/arduino_wire/i2c.cpp"
#include "sys/arduino_wire/freq_count.cpp"

#elif defined(TRB_AS3935_ARDUINO_BRZO)
#include "sys/arduino_wire/delay.cpp"
#include "sys/arduino_brzo/i2c.cpp"
#include "sys/arduino_wire/freq_count.cpp"
#endif

#endif // !defined(TRB_AS3935_cpp)
