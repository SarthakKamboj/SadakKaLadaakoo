//
//  skl_math.h
//  SadakKaLadaakoo-Mac
//
//  Created by Sarthak Kamboj on 12/14/24.
//

#include <simd/simd.h>

struct vector_t {
    union {
        struct {
            float x;
            float y;
            float z;
            float w;
        };
        float vals[4];
    };
    
};

struct matrix_t {
    union {
        struct {
            vector_t row0;
            vector_t row1;
            vector_t row2;
            vector_t row3;
        };
        vector_t rows[4];
    };
    
};

matrix_t create_mat();
matrix_t transform_mat(float z_offset,float scale);
matrix_t pers_mat(float near_plane, float far_plane);
matrix_t ndc_mat(float near_plane, float far_plane, float fov, float aspect_ratio);

simd::float4x4 matrix_to_simd_mat(matrix_t& mat);
