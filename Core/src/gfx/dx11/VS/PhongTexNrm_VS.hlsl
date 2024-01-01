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
    float3 tangent : Tangent;
    float3 bitangent : Bitangent;
    float2 tc : Texcoord;
    float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float3 n : Normal, float3 tan : Tangent, float3 bitan : Bitangent, float2 tc : Texcoord)
{
    const matrix modelView = mul(model, model);
    const matrix modelViewProj = mul(model, viewProjection);
    VSOut vso;
    vso.viewPos = (float3) mul(float4(pos, 1.0f), modelView);
    vso.viewNormal = mul(n, (float3x3) modelView);
    vso.pos = mul(float4(pos, 1.0f), modelViewProj);
    vso.tangent = mul(tan, (float3x3) modelView);
    vso.bitangent = mul(bitan, (float3x3) modelView);
    vso.tc = tc;
    return vso;
}