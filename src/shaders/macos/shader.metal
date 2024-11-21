//
//  shader.metal
//  SadakKaLadaakoo-Mac
//
//  Created by Sarthak Kamboj on 11/19/24.
//

#include <metal_stdlib>
using namespace metal;

#include "../../geometry.h"

struct vs_out_t {
    float4 position [[position]];
    float3 color;
};

vs_out_t vertex vertex_main(uint vertexId [[vertex_id]], device const skl_vert_t* vertices [[buffer(0)]]) {
    vs_out_t out;
    out.position = float4(vertices[vertexId].pos.x, vertices[vertexId].pos.y, vertices[vertexId].pos.z, 1.0);
    out.color = float3(vertices[vertexId].color.r, vertices[vertexId].color.g, vertices[vertexId].color.b);
    return out;
}

half4 fragment fragment_main(vs_out_t in [[stage_in]] ) {
    return half4(in.color.r, in.color.g, in.color.b, 1.0);
}
