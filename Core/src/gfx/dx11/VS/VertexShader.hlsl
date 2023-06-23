struct VSOut
{
    float3 color : COLOR;
    float4 pos : SV_Position;
};

cbuffer CBuf
{
    matrix transform;
};

VSOut main( float3 pos : POSITION, float3 color : COLOR )
{
    VSOut vso;
    vso.pos = mul(float4(pos.x, pos.y, pos.z, 1.0f), transform);
    vso.color = color;
    return vso;
}