#ifndef UNICODE
#define UNICODE
#endif

#include <cstdio>
#include <stdio.h>

#include <windows.h>
#include <WindowsX.h>

#include "platform/app.h"
#include "platform/win32/skl_win32.h"

int WINAPI wWinMain(HINSTANCE h_instance, HINSTANCE disregard, PWSTR p_cmd_line_args, int n_show_state) { 

  while(true) {}

  win32_init_ctx_t win32_init_ctx{};
  win32_init_ctx.h_instance = h_instance;
  win32_init_ctx.p_cmd_line_args = p_cmd_line_args;
  win32_init_ctx.n_show_state = n_show_state;

  init(win32_init_ctx);
  run();

  return 0;
}
