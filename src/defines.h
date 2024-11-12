#pragma once

#include <stdio.h>

#define __SKL_OUT__
#define SKL_LOG(test, ...) { char tempBuffer[256]{}; sprintf_s(tempBuffer, 256, test, ##__VA_ARGS__); printf("\n[%s] %s", __FUNCTION__, tempBuffer); }
