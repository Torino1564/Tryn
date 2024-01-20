#include "BoneTransform.hlsli"

cbuffer transformation
{
    matrix model;
    matrix view;
    matrix viewProjection;
};

struct VSOut
{
    float3 viewPos : Position;
    float3 viewNormal : Normal;
    float3 tangent : Tangent;
    float3 bitangent : Bitangent;
    float2 tc : Texcoord;
    float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float3 n : Normal, float3 tan : Tangent, float3 bitan : Bitangent, float2 tc : Texcoord, uint4 boneIds : BoneIds, float4 boneWeights : BoneWeights)
{
    const matrix modelView = mul(model, view);
    const matrix modelViewProj = mul(model, viewProjection);
    
    float4 position = float4(pos, 1.0f);
    
    position = CalculateBoneTransformedPosition(boneIds, boneWeights, position);
    n = CalculateBoneTransformedNormal(boneIds, boneWeights, n);
    TangentAndBitangent tAndBt = CalculateBoneTransformedTandBT(boneIds, boneWeights, tan, bitan);
    
    VSOut vso;
    vso.viewPos = (float3) mul(position , modelView);
    vso.viewNormal = mul(n, (float3x3) modelView);
    vso.pos = mul(position, modelViewProj);
    vso.tangent = mul(tAndBt.tangent, (float3x3) modelView);
    vso.bitangent = mul(tAndBt.bitangent, (float3x3) modelView);
    vso.tc = tc;
    return vso;
}