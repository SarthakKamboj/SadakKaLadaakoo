#pragma once

#include "../app_state.h"

struct window_info_t {
  int width = 0;
  int height = 0;
  char name[256]{};
};

void poll_events();
