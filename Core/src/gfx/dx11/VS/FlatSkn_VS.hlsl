#include "BoneTransform.hlsli"

cbuffer transformation
{
    matrix model;
    matrix view;
    matrix viewProjection;
};

float4 main(float3 pos : POSITION , uint4 boneIds : BoneIds, float4 boneWeights : BoneWeights) : SV_POSITION
{
    float4 position = float4(pos, 1.0f);
    
    position = CalculateBoneTransformedPosition(boneIds, boneWeights, position);
    
    return mul(position, mul(model, viewProjection));
}