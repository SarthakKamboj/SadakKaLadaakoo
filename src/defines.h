#pragma once

#include <stdio.h>
#include <assert.h>

#import <Foundation/Foundation.h>

#define __SKL_OUT__

#if defined(SKL_WINDOWS)
#define SKL_LOG(test, ...) { char temp_buffer[256]{}; sprintf_s(temp_buffer, 256, test, ##__VA_ARGS__); printf("\n[%s] %s", __FUNCTION__, temp_buffer); }
#elif defined(SKL_MAC)
#define SKL_LOG(test, ...) { char temp_buffer[256]{}; snprintf(temp_buffer, 256, test, ##__VA_ARGS__); printf("\n[%s] %s", __FUNCTION__, temp_buffer); }
#endif

#define skl_assert_msg(msg) assert(false, msg)
#define skl_assert(exp, msg) assert(exp, msg)

NSString* c_string_to_ns_string(const char* c_str);
const char* ns_string_to_c_string(NSString* ns_str);
