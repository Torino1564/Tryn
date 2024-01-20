#include "BoneTransform.hlsli"

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

struct VSOut
{
	float3 viewPos : Position;
	float3 viewNormal : Normal;
	float4 pos : SV_Position;
};

VSOut main(float3 pos : POSITION, float3 n : normal, uint instanceID : SV_InstanceID)
{
    const matrix modelView = mul(modelArray[instanceID], view);
    const matrix modelViewProj = mul(modelArray[instanceID], viewProjection);
	VSOut vso;
	vso.viewPos = (float3)mul(float4(pos, 1.0f), modelView);
	vso.viewNormal = mul(n, (float3x3)modelView);
	vso.pos = mul(float4(pos, 1.0f), modelViewProj);
	return vso;
}