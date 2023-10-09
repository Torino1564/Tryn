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
	float2 tc : Texcoord;
};

VSOut main(float3 pos : Position, float3 n : Normal, float2 tc : Texcoord)
{
	VSOut vso;
	vso.viewPos = (float3)mul(float4(pos, 1.0f), model);
	vso.viewNormal = mul(n, (float3x3)model);
	vso.pos = mul(float4(pos, 1.0f), modelViewProj);
	vso.tc = tc;
	return vso;
}