#pragma once

struct transform_t {
    float screen_x_pos = 0;
    float screen_y_pos = 0;
};
int create_transform();
transform_t* get_transform_from_id(int id);

struct character_t {
    int transform_id = 0;
    int render_options_id = 0;
};