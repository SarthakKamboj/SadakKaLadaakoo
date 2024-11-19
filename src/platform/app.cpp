#include "app.h"

#include "win32/skl_win32.h"
#include "platform/app.h"

app_state_t g_app_state;

void init(const platform_init_ctx_t& init_ctx) {
    window_info_t window_info{};
    window_info.width = 800;
    window_info.height = 450;
    const char* name = "SadakKaLadaakoo";
    memcpy(window_info.name, name, strlen(name)+1);

    init_platform_specific(window_info, init_ctx);
}

void update() {

}

void run() {
    g_app_state.running = true;
    while (g_app_state.running) {
        poll_events();
        update();
        render_frame();
    }
}