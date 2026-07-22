Texture2D SceneTexture : register(t0);
SamplerState Sampler : register(s0);

cbuffer StateParam : register(b0)
{
	float time;
	float3 padding;
};

float4 PS_Main(float4 pos : SV_POSITION, float2 uv : TEXCOORD0) : SV_TARGET
{
	float4 col = SceneTexture.Sample(Sampler, uv);
	return col;
}
