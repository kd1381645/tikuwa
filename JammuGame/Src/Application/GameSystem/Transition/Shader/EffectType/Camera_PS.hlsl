Texture2D SceneTexture : register(t0);
SamplerState Sampler : register(s0);

cbuffer StateParam : register(b0)
{
	float time;
	float3 padding;
};

float rand(float2 uv)
{
	return frac(sin(dot(uv, float2(12.9898, 78.233))) * 43758.5453);
}

float4 PS_Main(float4 pos : SV_POSITION, float2 uv : TEXCOORD0) : SV_Target
{
	float4 col = SceneTexture.Sample(Sampler, uv);

    // 輝度計算
	float gray = dot(col.rgb, float3(0.299, 0.587, 0.114));

    // 茶色セピア（古い地図）
	float3 warm;
	warm.r = gray * 1.15 + 0.10;
	warm.g = gray * 1.00 + 0.05;
	warm.b = gray * 0.60 - 0.02;
	col.rgb = saturate(warm);

    // コントラストを落として色あせた感じに
	col.rgb = lerp(col.rgb, float3(0.5, 0.5, 0.5), 0.10);

    // 全体を少し明るく（古い紙の白っぽさ）
	col.rgb += 0.1;

    // 羊皮紙のムラ
	float2 coarseUV = floor(uv * 80.0) / 80.0;
	col.rgb += rand(coarseUV + frac(time * 0.01)) * 0.06 - 0.03;

    // ビネット（周辺減光）
	float2 center = uv - 0.5;
	float vignette = saturate(1.0 - dot(center, center) * 2.2);
	col.rgb *= vignette;

    // 端を焦げ茶に寄せる
	float3 vignetteColor = float3(0.25, 0.15, 0.05);
	col.rgb = lerp(vignetteColor, col.rgb, vignette * 0.8 + 0.2);
	return saturate(col);
}
