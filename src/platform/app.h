#pragma once

struct window_info_t {
  int width = 0;
  int height = 0;
  char name[256]{};
};

struct input_t {
  // top left is (0,0)
  float mouse_x = 0;
  float mouse_y = 0;
};

struct app_state_t {
  input_t input;
  bool running = false;
};

#if defined(SKL_WINDOWS)
struct win32_init_ctx_t;
typedef win32_init_ctx_t platform_init_ctx_t;
#elif defined(SKL_MAC)
struct mac_init_ctx_t;
typedef mac_init_ctx_t platform_init_ctx_t;
#endif

void app_run(const platform_init_ctx_t& init_ctx);

void update();

struct render_options_t {
  float color[3] = {0,0,0};
};
int create_render_options();
render_options_t* get_render_options(int id);