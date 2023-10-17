#include "LightVector.hlsli"
#include "Operations.hlsli"

cbuffer PointLightCBuf : register(b0)
{
    float3 viewLightPos;
    float3 ambient;
    float3 diffuseColor;
    float diffuseIntensity;
    float constantAtt;
    float linearAtt;
    float quadraticAtt;
};

cbuffer ObjectCBuf : register(b1)
{
    float3 specularColor;
    float specularWeight;
    float specularGloss;
    bool useNormalMap;
    float normalMapWeight;
};

Texture2D tex : register(t0);
Texture2D norm : register(t2);

SamplerState splr : register(s0);

float4 main(const float3 viewPos : POSITION, float3 viewNormal : NORMAL, const float3 viewTangent : TANGENT, const float3 viewBitangent : BITANGENT, const float2 tc : Texcoord, const float4 pos : SV_POSITION) : SV_TARGET
{
    // normalize the mesh normal
    viewNormal = normalize(viewNormal);
    // replace normal with mapped if normal mapping enabled
    if (useNormalMap)
    {
        const float3 mappedNormal = MapNormal(normalize(viewTangent), normalize(viewBitangent), viewNormal, tc, norm, splr);
        viewNormal = lerp(viewNormal, mappedNormal, normalMapWeight);
    }
    
    const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewPos);

    const float attenuation = Attenuate(constantAtt, linearAtt, quadraticAtt, lv.distToL);
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, attenuation, lv.dirToL, viewNormal);

    float3 specular = Speculate(diffuseColor * specularColor, specularWeight, viewNormal, lv.vToL, viewPos, attenuation, specularGloss);

    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specular), 1.0f);
}