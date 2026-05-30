cbuffer transformation : register(b0)
{
    matrix model;
    matrix view;
    matrix viewProjection;
};

cbuffer transformationArray : register(b2)
{
    matrix modelArray[1023];
};

float4 main(float3 pos : POSITION, uint instanceID : SV_InstanceID) : SV_POSITION
{
    return mul(float4(pos, 1.0f), mul(modelArray[instanceID], viewProjection));
}