#include <cstdio>
#include <stdio.h>

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <WindowsX.h>

#include <stdint.h>

#include "gfx_api/windows/skl_win32.h"
#include "defines.h"
#include "platform/app.h"

extern app_state_t g_app_state;

int WINAPI wWinMain(HINSTANCE h_instance, HINSTANCE disregard, PWSTR p_cmd_line_args, int n_show_state) {

#if false
  window_info_t window_info{};
  window_info.width = 800;
  window_info.height = 450;
  const char* name = "SadakKaLadaakoo";
  memcpy(window_info.name, name, strlen(name)+1);
#endif

  win32_init_ctx_t win32_init_ctx{};
  win32_init_ctx.h_instance = h_instance;
  win32_init_ctx.p_cmd_line_args = p_cmd_line_args;
  win32_init_ctx.n_show_state = n_show_state;

  // init_platform_specific(window_info, win32_init_ctx);

  init(win32_init_ctx);

  g_app_state.running = true;
  while (g_app_state.running) {
    poll_events();
    // render_frame(render_ctx);
    render_frame();
  }

  return 0;
}
