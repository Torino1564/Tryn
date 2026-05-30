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
	float2 tc : Texcoord;
	float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float3 n : Normal, float2 tc : Texcoord, uint4 boneIds : BoneIds, float4 boneWeights : BoneWeights)
{
    float4 position = float4(pos, 1.0f);
	
    position = CalculateBoneTransformedPosition(boneIds, boneWeights, position);
    n = CalculateBoneTransformedNormal(boneIds, boneWeights, n);
	
    const matrix modelView = mul(model, view);
    const matrix modelViewProj = mul(model, viewProjection);
	VSOut vso;
	vso.viewPos = (float3)mul(position, modelView);
	vso.viewNormal = mul(n, (float3x3)modelView);
	vso.pos = mul(position, modelViewProj);
	vso.tc = tc;
	return vso;
}