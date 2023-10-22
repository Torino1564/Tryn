#include "LightVector.hlsli"
#include "Operations.hlsli"

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

float4 main(const float3 viewPos : POSITION, float3 viewNormal : NORMAL) : SV_TARGET
{
    viewNormal = normalize(viewNormal);
	const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewPos);
	
    const float attenuation = Attenuate(constantAtt, linearAtt, quadraticAtt, lv.distToL);
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, attenuation, lv.dirToL, viewNormal);
    const float3 specular = Speculate(specularColor, specularWeight, viewNormal, lv.vToL, viewPos, attenuation, specularGloss);
	
    return float4(saturate((diffuse + ambient) * materialColor + specular), 1.0f);
}