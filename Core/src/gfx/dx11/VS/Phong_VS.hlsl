cbuffer transformation
{
	matrix model;
	matrix modelView;
	matrix modelViewProj;
};

struct VSOut
{
	float3 viewPos : Position;
	float3 viewNormal : Normal;
	float4 pos : SV_Position;
};

VSOut main(float3 pos : POSITION, float3 n : normal)
{
	VSOut vso;
	vso.viewPos = (float3)mul(float4(pos, 1.0f), model);
	vso.viewNormal = mul(n, (float3x3)model);
	vso.pos = mul(float4(pos, 1.0f), modelViewProj);
	return vso;
}