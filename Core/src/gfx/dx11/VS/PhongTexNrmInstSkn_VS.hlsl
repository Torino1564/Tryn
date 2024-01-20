cbuffer transformation : register(b0)
{
    matrix model;
    matrix view;
    matrix viewProjection;
};

cbuffer transformationArray : register(b2)
{
    matrix modelArray[1023];
};

struct VSOut
{
    float3 viewPos : Position;
    float3 viewNormal : Normal;
    float3 tangent : Tangent;
    float3 bitangent : Bitangent;
    float2 tc : Texcoord;
    float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float3 n : Normal, float3 tan : Tangent, float3 bitan : Bitangent, float2 tc : Texcoord, uint instanceID : SV_InstanceID)
{
    const matrix modelView = mul(modelArray[instanceID], view);
    const matrix modelViewProj = mul(modelArray[instanceID], viewProjection);
    VSOut vso;
    vso.viewPos = (float3) mul(float4(pos, 1.0f), modelView);
    vso.viewNormal = mul(n, (float3x3) modelView);
    vso.pos = mul(float4(pos, 1.0f), modelViewProj);
    vso.tangent = mul(tan, (float3x3) modelView);
    vso.bitangent = mul(bitan, (float3x3) modelView);
    vso.tc = tc;
    return vso;
}