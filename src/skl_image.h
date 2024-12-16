//
//  skl_image.h
//  SadakKaLadaakoo-Mac
//
//  Created by Sarthak Kamboj on 12/16/24.
//

#include "Metal/Metal.h"

struct skl_image_t {
    bool valid = false;
    id<MTLTexture> texture;
};

skl_image_t load_image(const char* path, id<MTLDevice> device);
