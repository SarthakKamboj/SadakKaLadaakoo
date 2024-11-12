#pragma once

#include "../app_state.h"

struct WindowInfo {
  int width = 0;
  int height = 0;
  char name[256]{};
};

void PollEvents();
