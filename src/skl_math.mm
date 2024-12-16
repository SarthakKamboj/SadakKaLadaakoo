//
//  skl_math.mm
//  SadakKaLadaakoo-Mac
//
//  Created by Sarthak Kamboj on 12/14/24.
//

#include "skl_math.h"
#include <cmath>
#include <memory>

matrix_t create_mat() {
    matrix_t mat;
    memset(&mat, 0, sizeof(mat));
    return mat;
}

matrix_t transform_mat(float z_offset, float scale) {
    matrix_t mat = create_mat();
    mat.row0.x = scale;
    mat.row1.y = scale;
    mat.row2.z = scale;
    mat.row2.w = z_offset;
    mat.row3.w = 1.f;
    return mat;
}

matrix_t pers_mat(float near_plane, float far_plane) {
    matrix_t mat = create_mat();
    mat.row0.x = near_plane;
    mat.row1.y = near_plane;
    mat.row2.z = near_plane + far_plane;
    mat.row2.w = -near_plane * far_plane;
    mat.row3.z = 1;
    return mat;
}

matrix_t ndc_mat(float near_plane, float far_plane, float fov, float aspect_ratio) {
    
    float right = near_plane * tanf(fov * 180.f / 3.141527f);
    float top = right / aspect_ratio;
    
    matrix_t mat = create_mat();
    mat.row0.x = 1 / right;
    mat.row1.y = 1 / top;
    mat.row2.z = 1 / (far_plane - near_plane);
    mat.row2.w = -near_plane / (far_plane - near_plane);
    mat.row3.w = 1;
    
    return mat;
}

vector3_t cross_product(vector3_t a, vector3_t b) {
    vector3_t cross;
    
    cross.x = a.y * b.z - a.z * b.y;
    cross.y = a.z * b.x - a.x * b.z;
    cross.z = a.x * b.y - a.y * b.x;
    
    return cross;
}

matrix_t cam_mat(vector3_t cam_pos, vector3_t cam_dir) {
    matrix_t mat;
    
    vector3_t x_basis;
    vector3_t y_basis;
    vector3_t z_basis;
    
    z_basis = {cam_dir.x, cam_dir.y, cam_dir.z};
    x_basis = cross_product(z_basis, {0, 1.f, 0});
    y_basis = cross_product(z_basis, x_basis);
    
    mat.row0.x = x_basis.x;
    mat.row1.x = x_basis.y;
    mat.row2.x = x_basis.z;
    
    mat.row0.y = y_basis.x;
    mat.row1.y = y_basis.y;
    mat.row2.y = y_basis.z;
    
    mat.row0.z = z_basis.x;
    mat.row1.z = z_basis.y;
    mat.row2.z = z_basis.z;
    
    mat.row0.w = -cam_pos.x;
    mat.row1.w = -cam_pos.y;
    mat.row2.w = -cam_pos.z;
    mat.row3.w = 1.f;

    return mat;
}

simd::float4x4 matrix_to_simd_mat(matrix_t& mat) {
    simd::float4x4 sm;
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            sm.columns[col][row] = mat.rows[row].vals[col];
        }
    }
    return sm;
}
