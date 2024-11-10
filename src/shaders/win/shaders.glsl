struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

/*
cbuffer constants_t : register(b2) 
{
    float offset;  
};
*/

cbuffer my_constants : register(b0)
{
    float mouse_x;  
    float mouse_y;  
};

PSInput VSMain(float4 position : POSITION, float4 color : COLOR)
{
    PSInput result;

    result.position = position;
    result.position.x += mouse_x;
    result.position.y += mouse_y;
    result.position.w = 1.0;
    result.color = color;

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return input.color;
}
