Import("env")
src_filter = ["+<TRB_AS3935.h>"]
env.Replace(SRC_FILTER=src_filter)

build_flags = env.ParseFlags(env['BUILD_FLAGS'])
cppdefines = build_flags.get("CPPDEFINES")

if "TRB_AS3935_ESP_IDF" in cppdefines:
    env.Append(SRC_FILTER=["+<TRB_AS3935.c>"])
if "TRB_AS3935_ARDUINO_WIRE" in cppdefines:
    env.Append(SRC_FILTER=["+<TRB_AS3935.cpp>"])
if "TRB_AS3935_ARDUINO_BRZO" in cppdefines:
    env.Append(SRC_FILTER=["+<TRB_AS3935.cpp>"])
#print env.get("SRC_FILTER")
