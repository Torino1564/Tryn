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
#ifndef NoSpc
    bool useSpecularAlpha;
    bool useSpecularMap;
#endif
#ifdef NoTex
	float3 materialColor;
#endif
    float3 specularColor;
    float specularWeight;
    float specularGloss;
#ifndef NoNrm
    bool useNormalMap;
    float normalMapWeight;
#endif
};

struct PS_Output
{
    float4 Color : SV_Target0;
};

#ifndef NoTex
Texture2D tex : register(t0);
#endif

#ifndef NoNrm
Texture2D norm : register(t2);
#endif

#ifndef NoSpc
Texture2D spec : register(t1);
#endif


#if !defined(NoSpc) || !defined(NoNrm) || !defined(NoTex)
SamplerState splr : register(s0);
#endif

PS_Output main(const float3 viewPos : POSITION, float3 viewNormal : NORMAL, const float3 viewTangent : TANGENT, const float3 viewBitangent : BITANGENT, const float2 tc : Texcoord) : SV_TARGET
{
#ifndef NoTex
    const float4 diffuseSample = tex.Sample(splr, tc);
    const float3 diffuseColor3 = diffuseSample.rgb;
#else
	const float3 diffuseColor3 = materialColor;
#endif

#ifdef MASK
    // bail if highly translucent
    clip(diffuseSample.a < 0.1f ? -1 : 1);
    // flip normal when backface
    if (dot(viewNormal, viewPos) >= 0.0f)
    {
        viewNormal = -viewNormal;
    }
#endif
    
    // normalize the mesh normal
    viewNormal = normalize(viewNormal);

#ifndef NoNrm
    // replace normal with mapped if normal mapping enabled
    if (useNormalMap)
    {
        const float3 mappedNormal = MapNormal(normalize(viewTangent), normalize(viewBitangent), viewNormal, tc, norm, splr);
        viewNormal = lerp(viewNormal, mappedNormal, normalMapWeight);
    }
#endif

    const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewPos);

    const float attenuation = Attenuate(constantAtt, linearAtt, quadraticAtt, lv.distToL);
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, attenuation, lv.dirToL, viewNormal);


    float specularPower = specularGloss;

    float3 specularColor3 = specularColor;
#ifndef NoSpc
    const float4 specularSample = spec.Sample(splr, tc);

    if (useSpecularMap)
    {
        specularColor3 = specularSample.rgb;
    }

    if (useSpecularAlpha)
    {
        specularPower = pow(2.0f, specularSample.a * 13.0f); 
    }
    else
    {
        specularPower = specularGloss;
    }
#endif

    float3 specular = Speculate(diffuseColor * specularColor3, specularWeight, viewNormal, lv.vToL, viewPos, attenuation, specularPower);

    PS_Output output;

    output.Color = float4(saturate((diffuse + ambient) * diffuseColor3 + specular), 1.0f);

    return output;
}