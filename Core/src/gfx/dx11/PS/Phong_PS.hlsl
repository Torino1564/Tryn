#include "LightVector.hlsli"

cbuffer PointLightCBuf : register(b0)
{
	float3	 _viewLightPos;
	float3	 _ambient;
	float3	 _diffuseColor;
	float	 _diffuseIntensity;
	float	 _constantAtt;
	float	 _linearAtt;
	float	 _quadraticAtt;
};

cbuffer ObjectCBuf : register(b1)
{
	float3 materialColor;
	float3 specularColor;
	float specularWeight;
	float specularGloss;
};

float4 main(const float3 viewPos : POSITION, const float3 viewNormal : NORMAL) : SV_TARGET
{
    float3 viewLightPos = float3(0, 0, 10);
    float3 ambient = float3(0, 0, 0.5f);
    float3 diffuseColor = float3(0.5f, 0, 0);
    float diffuseIntensity = 0.8f;
    float constantAtt = 0.5;
    float linearAtt = 0.003;
    float quadraticAtt = 0.00075;
	
	const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewPos);

	const float attenuation = 1.0f / (constantAtt + linearAtt * lv.distToL + quadraticAtt * pow(lv.dirToL, 2));
	const float3 diffuse = diffuseColor * diffuseIntensity * attenuation * max(0.0f, dot(lv.dirToL, viewNormal));

	const float3 w = viewNormal * dot(lv.vToL, viewNormal);
	const float3 r = normalize(w * 2.0f - lv.vToL);
	// vector from camera to fragment (in view space)
	const float3 viewCamToFrag = normalize(viewPos);
	// calculate specular component color based on angle between
	// viewing vector and reflection vector, narrow with power function
	const float3 specular = attenuation * specularColor * specularWeight * pow(max(0.0f, dot(-r, viewCamToFrag)), specularGloss);

	return float4(saturate((diffuse + ambient) * materialColor + specular), 1.0f);
}