//
//  skl_math.h
//  SadakKaLadaakoo-Mac
//
//  Created by Sarthak Kamboj on 12/14/24.
//

#include <simd/simd.h>

struct vector4_t {
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

struct vector3_t {
    union {
        struct {
            float x;
            float y;
            float z;
        };
        float vals[3];
    };
};

struct matrix_t {
    union {
        struct {
            vector4_t row0;
            vector4_t row1;
            vector4_t row2;
            vector4_t row3;
        };
        vector4_t rows[4];
    };
    
};

vector3_t cross_product(vector3_t v1, vector3_t v2);

matrix_t create_mat();
matrix_t transform_mat(float z_offset,float scale);
matrix_t pers_mat(float near_plane, float far_plane);
matrix_t ndc_mat(float near_plane, float far_plane, float fov, float aspect_ratio);
matrix_t cam_mat(vector3_t cam_pos, vector3_t cam_dir);

simd::float4x4 matrix_to_simd_mat(matrix_t& mat);
