#include "LightVector.hlsli"
#include "Operations.hlsli"

#define MAX_POINT_LIGHTS 2

struct PointLightParams
{
    float3 viewLightPos;
    float3 diffuseColor;
    float diffuseIntensity;
    float constantAtt;
    float linearAtt;
    float quadraticAtt;
};

cbuffer PointLightCBuf : register(b0)
{
    uint numPointLights;
    float3 ambient;
    PointLightParams pointLights[MAX_POINT_LIGHTS];
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

float4 main(    const float3 viewPos : POSITION
				,float3 viewNormal : NORMAL
#ifndef NoNrm
				,const float3 viewTangent : TANGENT
				,const float3 viewBitangent : BITANGENT
#endif
#ifndef NoTex
				,const float2 tc : Texcoord
#endif
			) : SV_TARGET
{
#ifndef NoTex
    float4 diffuseSample = tex.Sample(splr, tc);
    float3 diffuseColor3 = diffuseSample.rgb;
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

    float3 diffuse = { 0.0f, 0.0f, 0.0f };
    float3 specular = { 0.0f, 0.0f, 0.0f };
    float specularPower = { 0.0f };
    for (uint i = 0; i < numPointLights; i++)
    {
        if (i >= numPointLights)
            break;
        const PointLightParams light = pointLights[i];
        const LightVectorData lv = CalculateLightVectorData(light.viewLightPos, viewPos);

        const float attenuation = Attenuate(light.constantAtt, light.linearAtt, light.quadraticAtt, lv.distToL);
        diffuse += Diffuse(light.diffuseColor, light.diffuseIntensity, attenuation, lv.dirToL, viewNormal);

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

        specular += Speculate(light.diffuseColor * specularColor3, specularWeight, viewNormal, lv.vToL, viewPos, attenuation, specularPower);
    }

    return float4(saturate((diffuse + ambient) * diffuseColor3), 1.0f);
}