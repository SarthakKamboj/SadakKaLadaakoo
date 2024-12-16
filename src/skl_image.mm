//
//  skl_image.mm
//  SadakKaLadaakoo-Mac
//
//  Created by Sarthak Kamboj on 12/16/24.
//

#include "skl_image.h"
#include "stb_image.h"

skl_image_t load_image(const char* path, id<MTLDevice> device) {
    
    skl_image_t img;
    
    int width, height, channels;
    unsigned char* data = stbi_load(path, &width, &height, &channels, 0);
    printf("image %s is %i by %i and has %i channels", path, width, height, channels);
    uint8_t* formatted_data = (uint8_t*)malloc(sizeof(uint8_t)*width*height*4);
    if (channels == 3) {
        int data_idx = 0;
        int formatted_idx = 0;
        for (int row = 0; row < height; row++) {
            for (int col = 0; col < width; col++) {
                uint8_t red = data[data_idx++];
                uint8_t green = data[data_idx++];
                uint8_t blue = data[data_idx++];
                
                formatted_data[formatted_idx++] = blue;
                formatted_data[formatted_idx++] = green;
                formatted_data[formatted_idx++] = red;
                formatted_data[formatted_idx++] = 255;

            }
        }
        MTLTextureDescriptor* tex_desc = [[MTLTextureDescriptor alloc] init];
        tex_desc.pixelFormat = MTLPixelFormatBGRA8Unorm;
        tex_desc.width = width;
        tex_desc.height = height;
        
        img.texture = [device newTextureWithDescriptor:tex_desc];
        MTLRegion region = {
            {0,0,0},
            {static_cast<NSUInteger>((NSInteger)width), static_cast<NSUInteger>((NSInteger)height), 1}
        };
        NSInteger bytes_per_row = sizeof(uint8_t)*4*width;
        [img.texture replaceRegion:region mipmapLevel:0 withBytes:formatted_data bytesPerRow:bytes_per_row];
        img.valid = true;
        
    } else {
        printf("can't handle this many channels");
    }
    free(formatted_data);
    stbi_image_free(data);
    
    return img;
}
