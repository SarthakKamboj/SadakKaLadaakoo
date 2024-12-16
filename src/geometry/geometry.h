#pragma once

#include "simd/simd.h"

struct skl_pos_t {
  float x = 0;
  float y = 0;
  float z = 0;
};

struct skl_color_t {
  float r = 0;
  float g = 0;
  float b = 0;
  float a = 1.0f;
};

struct skl_vert_t {
    skl_pos_t pos;
    skl_color_t color;
    vector_float2 uv;
};
