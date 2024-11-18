#pragma once

#include <stdio.h>
#include <assert.h>

#define __SKL_OUT__
#define SKL_LOG(test, ...) { char temp_buffer[256]{}; sprintf_s(temp_buffer, 256, test, ##__VA_ARGS__); printf("\n[%s] %s", __FUNCTION__, temp_buffer); }

#define skl_assert_msg(msg) assert(false, msg)
#define skl_assert(exp, msg) assert(exp, msg)

#define min(a,b) ( a < b ? a : b )