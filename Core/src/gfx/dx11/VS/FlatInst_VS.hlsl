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

uint instanceID : SV_InstanceID;

float4 main(float3 pos : POSITION) : SV_POSITION
{
    return mul(float4(pos, 1.0f), mul(modelArray[instanceID], viewProjection));
}