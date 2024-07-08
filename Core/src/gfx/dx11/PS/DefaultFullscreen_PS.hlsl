Texture2D tex;
SamplerState splr;

float4 main(const float2 uv : Texcoord) : SV_TARGET
{
	return tex.Sample(splr, uv).rgba;
}