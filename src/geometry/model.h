//
//  model.h
//  SadakKaLadaakoo-Mac
//
//  Created by Sarthak Kamboj on 12/12/24.
//

#include "geometry.h"

struct model_t {
    skl_vert_t* verts = nullptr;
    int num_verts = -1;
};

model_t load_model(const char* model_path);
