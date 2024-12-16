//
//  shader.metal
//  SadakKaLadaakoo-Mac
//
//  Created by Sarthak Kamboj on 11/19/24.
//

#include <metal_stdlib>
using namespace metal;

#include "../../geometry/geometry.h"
#include "shader.h"

struct vs_out_t {
    float4 position [[position]];
    float3 color;
    float2 uv;
};

vs_out_t vertex vertex_main(
    uint vertexId [[vertex_id]],
    device const skl_vert_t* vertices [[buffer(0)]],
    device const uniform_data_t* uniform_data [[buffer(1)]]
) {
    vs_out_t out;
    // out.position = float4(vertices[vertexId].pos.x + uniform_data->mouse_pos_x, vertices[vertexId].pos.y + uniform_data->mouse_pos_y, vertices[vertexId].pos.z, 1.0);
    out.position = uniform_data->ndc_mat * uniform_data->pers_mat * uniform_data->cam_mat * uniform_data->world_mat * float4(vertices[vertexId].pos.x, vertices[vertexId].pos.y, vertices[vertexId].pos.z, 1.0);
    // out.color = float3(vertices[vertexId].color.r, vertices[vertexId].color.g, vertices[vertexId].color.b);
    out.color = float3(uniform_data->color[0], uniform_data->color[1], uniform_data->color[2]);
    out.uv = vertices[vertexId].uv;
    return out;
}

half4 fragment fragment_main(vs_out_t in [[stage_in]], texture2d<half> img_texture [[ texture(0) ]]) {
    // return half4(in.color.r, in.color.g, in.color.b, 1.0);
    constexpr sampler tex_sampler (mag_filter::linear,
                                      min_filter::linear);
    return img_texture.sample(tex_sampler, in.uv);
}
