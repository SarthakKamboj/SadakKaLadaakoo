struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

cbuffer my_constants : register(b0)
{
    float2 mouse;
    float3 color; // starts new 4-component vector because of HLSL 4-component boundary packing rule
};

PSInput VSMain(float4 position : POSITION, float4 in_color : COLOR)
{
    PSInput result;

    result.position = position;
    result.position.x += mouse.x;
    result.position.y += mouse.y;
    result.position.w = 1.0;
    result.color = in_color;

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return float4(color.r, color.g, color.b, 1);
    // return input.color;
}
