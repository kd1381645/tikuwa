// Camera_PS_GameBoy.hlsl
Texture2D SceneTexture : register(t0);
SamplerState Sampler : register(s0);
cbuffer StateParam : register(b0)
{
	float time;
	float blendFactor;
	float2 padding;
};

float4 PS_Main(float4 pos : SV_POSITION, float2 uv : TEXCOORD0) : SV_Target
{
    // ① ピクセル化（160×144がゲームボーイの解像度）
	float2 pixelSize = float2(1.0 / 160.0, 1.0 / 144.0);
	float2 pixelUV = floor(uv / pixelSize) * pixelSize + pixelSize * 0.5;

	float4 col = SceneTexture.Sample(Sampler, pixelUV);

    // ② グレースケールに変換
	float gray = dot(col.rgb, float3(0.299, 0.587, 0.114));

    // ③ 4段階に量子化（ゲームボーイは4色）
	float quantized = floor(gray * 4.0) / 3.0; // 0, 0.33, 0.67, 1.0

    // ④ ゲームボーイの4色パレット
    // 暗→明: 濃緑→緑→薄緑→ほぼ白（実機の液晶色）
	float3 palette[4] =
	{
		float3(0.06, 0.22, 0.06), // 最暗（ほぼ黒緑）
        float3(0.19, 0.38, 0.19), // 暗
        float3(0.54, 0.68, 0.54), // 明
        float3(0.73, 0.80, 0.64), // 最明（液晶の緑白）
	};

	int idx = (int) (quantized * 3.0 + 0.5);
	float3 gbCol = palette[clamp(idx, 0, 3)];

    // ⑤ ドット間の隙間（液晶のピクセルグリッド）
	float2 dotPos = frac(uv / pixelSize);
	float gap = step(0.9, dotPos.x) + step(0.9, dotPos.y);
	gbCol *= saturate(1.0 - gap * 0.3);

    // ⑥ 液晶のバックライトムラ（軽いビネット）
	float2 center = uv - 0.5;
	float vignette = 1.0 - dot(center, center) * 0.8;
	gbCol *= saturate(vignette);

	return float4(gbCol, 1.0);
}
