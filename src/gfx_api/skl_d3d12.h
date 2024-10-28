#pragma once

#include <windows.h>


struct SKL_Position {
  float x = 0;
  float y = 0;
  float z = 0;

#if false
  SKL_Position() = default;

  SKL_Position(float _x, float _y, float _z) {
    x = _x;
    y = _y;
    z = _z;
  }
#endif
};

struct SKL_Color {
  float r = 0;
  float g = 0;
  float b = 0;
  float a = 1.0f;

#if false
  SKL_Color() = default;

  SKL_Color(float _r, float _g, float _b, float _a) {
    r = _r;
    g = _g;
    b = _b;
    a = _a;
  }
#endif
};

struct SKL_Vertex {
  SKL_Position pos;
  SKL_Color color;

#if false
  SKL_Vertex() = default;

  SKL_Vertex(SKL_Position _pos, SKL_Color _col) {
    pos = _pos;
    color = _col;
  }
#endif
};

void initD3D12(HWND hwnd);
