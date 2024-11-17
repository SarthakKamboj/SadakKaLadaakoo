#pragma once

#include <stdio.h>

#define __SKL_OUT__
#define SKL_LOG(test, ...) { char temp_buffer[256]{}; sprintf_s(temp_buffer, 256, test, ##__VA_ARGS__); printf("\n[%s] %s", __FUNCTION__, temp_buffer); }
