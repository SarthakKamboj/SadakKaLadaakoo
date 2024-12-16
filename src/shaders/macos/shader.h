//
//  shader.h
//  SadakKaLadaakoo-Mac
//
//  Created by Sarthak Kamboj on 11/22/24.
//

#import <simd/SIMD.h>

struct uniform_data_t {
    float mouse_pos_x;
    float mouse_pos_y;
    
    float color[3];
    simd::float4x4 world_mat;
    simd::float4x4 pers_mat;
    simd::float4x4 ndc_mat;
    simd::float4x4 cam_mat;
};

