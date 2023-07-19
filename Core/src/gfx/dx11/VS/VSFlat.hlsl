cbuffer CBuf
{
    matrix transform;
};

float4 main( float4 pos : POSITION , float4 norm : NORMAL) : SV_POSITION
{
    return mul(float4(pos.x, pos.y, pos.z, 1.0f), transform);
}