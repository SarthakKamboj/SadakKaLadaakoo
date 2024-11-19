#pragma once

struct window_info_t {
  int width = 0;
  int height = 0;
  char name[256]{};
};

struct app_state_t {
  float mouse_x = 0;
  float mouse_y = 0;
  bool running = false;
};

#if defined(SKL_WINDOWS)
struct win32_init_ctx_t;
typedef win32_init_ctx_t platform_init_ctx_t;
#elif defined(SKL_MAC)
struct mac_init_ctx_t;
typedef mac_init_ctx_t platform_init_ctx_t;
#endif

void init(const platform_init_ctx_t& init_ctx);

void run();

void poll_events();
void update();
void render_frame();
