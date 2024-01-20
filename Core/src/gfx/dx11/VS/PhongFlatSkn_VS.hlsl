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
	float4 pos : SV_Position;
};	

VSOut main(float3 pos : POSITION, float3 n : Normal, uint4 boneIds : BoneIDs, float4 boneWeights : BoneWeights)
{
    const matrix modelView = mul(model, view);
    const matrix modelViewProj = mul(model, viewProjection);
	
    float4 position = float4(pos, 1.0f);
	
	// bone calculation
    position = CalculateBoneTransformedPosition(boneIds, boneWeights, position);
    n = CalculateBoneTransformedNormal(boneIds, boneWeights, n);
	
	VSOut vso;
	vso.viewPos = (float3)mul(position, modelView);
    vso.viewNormal = mul(n, (float3x3)modelView);
	vso.pos = mul(position, modelViewProj);
	return vso;
}