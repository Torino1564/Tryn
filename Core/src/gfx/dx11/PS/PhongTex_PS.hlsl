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

Texture2D tex : register(t0);
SamplerState splr : register(s0);

float4 main(const float3 viewPos : POSITION, float3 viewNormal : NORMAL, const float2 tc : Texcoord ,const float4 pos : SV_POSITION) : SV_TARGET
{
    const float4 dtex = tex.Sample(splr, tc);
#ifdef MASK
    // bail if highly translucent
    clip(dtex.a < 0.1f ? -1 : 1);
    // flip normal when backface
    if (dot(viewNormal, viewPos) >= 0.0f)
    {
        viewNormal = -viewNormal;
    }
#endif
	
	const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewPos);
    viewNormal = normalize(viewNormal);
    const float attenuation = Attenuate(constantAtt, linearAtt, quadraticAtt, lv.distToL);
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, attenuation, lv.dirToL, viewNormal);
    const float3 specular = Speculate(specularColor, specularWeight, viewNormal, lv.vToL, viewPos, attenuation, specularGloss);
	
    return float4(saturate((diffuse + ambient) * dtex.rgb + specular), 1.0f);
}