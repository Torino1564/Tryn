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

VSOut main(float3 pos : POSITION, float3 n : normal)
{
    const matrix modelView = mul(model, model);
    const matrix modelViewProj = mul(model, viewProjection);
	VSOut vso;
	vso.viewPos = (float3)mul(float4(pos, 1.0f), modelView);
	vso.viewNormal = mul(n, (float3x3)modelView);
	vso.pos = mul(float4(pos, 1.0f), modelViewProj);
	return vso;
}