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
    float offset;  
};

PSInput VSMain(float4 position : POSITION, float4 color : COLOR)
{
    PSInput result;

    result.position = position;
    result.position.y += offset;
    result.position.w = 1.0;
    result.color = color;

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return input.color;
}
