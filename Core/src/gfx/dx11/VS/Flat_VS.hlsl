cbuffer transformation
{
    matrix model;
    matrix view;
    matrix viewProjection;
};

float4 main(float3 pos : POSITION) : SV_POSITION
{
    return mul(float4(pos, 1.0f), mul(model, viewProjection));
}