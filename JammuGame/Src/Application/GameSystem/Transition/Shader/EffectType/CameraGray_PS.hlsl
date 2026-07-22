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

	float gray = dot(col.rgb, float3(0.299, 0.587, 0.114));
	
	return float4(gray * 0.5f, gray * 0.5f, gray * 0.5f, col.a);
}
