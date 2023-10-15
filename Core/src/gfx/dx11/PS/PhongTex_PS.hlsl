#include "LightVector.hlsli"

cbuffer PointLightCBuf : register(b0)
{
	float3	 viewLightPos;
	float3	 ambient;
	float3	 diffuseColor;
	float	 diffuseIntensity;
	float	 constantAtt;
	float	 linearAtt;
	float	 quadraticAtt;
};

cbuffer ObjectCBuf : register(b1)
{
	float3 materialColor;
	float3 specularColor;
	float specularWeight;
	float specularGloss;
};

Texture2D tex : register(t0);
SamplerState splr : register(s0);

float4 main(const float3 viewPos : POSITION, const float3 viewNormal : NORMAL, const float2 tc : Texcoord ,const float4 pos : SV_POSITION) : SV_TARGET
{
	const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewPos);

	const float attenuation = 1.0f / (constantAtt + linearAtt * lv.distToL + quadraticAtt * pow(lv.distToL, 2));
	const float3 diffuse = diffuseColor * diffuseIntensity * attenuation * max(0.0f, dot(lv.dirToL, viewNormal));

	const float3 w = viewNormal * dot(lv.vToL, viewNormal);
	const float3 r = normalize(w * 2.0f - lv.vToL);
	// vector from camera to fragment (in view space)
	const float3 viewCamToFrag = normalize(viewPos);
	// calculate specular component color based on angle between
	// viewing vector and reflection vector, narrow with power function
	const float3 specular = attenuation * specularColor * specularWeight * pow(max(0.0f, dot(-r, viewCamToFrag)), specularGloss);

	//return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specular), 1.0f);
	return float4(saturate((diffuse + ambient) * tex.Sample(splr,tc).rgb), 1.0f);
}