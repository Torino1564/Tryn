cbuffer Phong
{
    float3 diffuseColor;
    float diffuseIntensity;
    
    float3 ambientColor;
    float3 materialColor;
    
    float3 lightPos;
    float constantAtt;
    float linearAtt;
    float quadraticAtt;
};

float4 main( float3 worldpos : POSITION , float3 normal : NORMAL ) : SV_TARGET
{
    const float3 lightDirection = lightPos - worldpos;
    const float3 distanceToLight = length(lightDirection);
    const float3 lightDirection_N = normalize(lightDirection);
    
    const float attenuation = 1.0f / (constantAtt + linearAtt * distanceToLight + quadraticAtt * pow(distanceToLight, 2));
    const float3 diffuse = diffuseColor * diffuseIntensity * attenuation * max(0.0f, dot(lightDirection_N , normal));

    return float4(saturate(diffuse + ambientColor), 1.0f);
}