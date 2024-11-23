#include "app.h"

#if defined(SKL_WINDOWS)
#include "win32/skl_win32.h"
#elif defined(SKL_MAC)
#include "macos/skl_macos.h"
#endif

#include <string>

#include "entities.h"

static character_t character;

app_state_t g_app_state;

void app_run(const platform_init_ctx_t& init_ctx) {
    window_info_t window_info{};
    window_info.width = 800;
    window_info.height = 450;
    const char* name = "SadakKaLadaakoo";
    memcpy(window_info.name, name, strlen(name)+1);

    character.transform_id = create_transform();
    character.render_options_id = create_render_options();

    app_run_platform_specific(window_info, init_ctx);
}

void update() {

    transform_t* transform = get_transform_from_id(character.transform_id);
    transform->screen_x_pos = g_app_state.input.mouse_x;
    transform->screen_y_pos = g_app_state.input.mouse_y;

    render_options_t* render_options = get_render_options(character.render_options_id);
    render_options->color[0] = 1.0f;
    render_options->color[1] = 1.0f;
    render_options->color[2] = 1.0f;
}

static render_options_t render_options[1];
int create_render_options() {
    return 0;
}

render_options_t* get_render_options(int id) {
    if (id != 0) return NULL;
    return render_options;
}

static transform_t transforms[1];
int create_transform() {
    return 0;
}

transform_t* get_transform_from_id(int id) {
    if (id != 0) return NULL;
    return transforms;
}
