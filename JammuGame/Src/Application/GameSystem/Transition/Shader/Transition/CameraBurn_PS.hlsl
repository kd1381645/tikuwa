// Camera_PS_Trans_BombSmokeDream.hlsl
// 最後の爆弾が爆発した煙が
// そのまま夢の紫煙に変わって意識を包む
Texture2D OldEffect : register(t0);
SamplerState Sampler : register(s0);
cbuffer StateParam : register(b0)
{
	float time;
	float blendFactor;
	float2 pad;
};

float hash(float2 p)
{
	return frac(sin(dot(p, float2(127.1, 311.7))) * 43758.5453);
}
float noise(float2 p)
{
	float2 i = floor(p), f = frac(p);
	f = f * f * (3. - 2. * f);
	return lerp(lerp(hash(i), hash(i + float2(1, 0)), f.x),
                lerp(hash(i + float2(0, 1)), hash(i + float2(1, 1)), f.x), f.y);
}
float fbm(float2 p)
{
	return noise(p) * .5 + noise(p * 2.1 + 3.7) * .25 + noise(p * 4.3) * .125;
}

float4 PS_Main(float4 pos : SV_POSITION, float2 uv : TEXCOORD0) : SV_Target
{
	float4 oldCol = OldEffect.Sample(Sampler, uv);
	float t = blendFactor;

    // ① 煙が画面下から湧き上がる（爆発後の煙幕）
	float2 smokeUV = float2(uv.x + fbm(float2(uv.y * 3.0 + time * 0.3, t * 2.0)) * 0.06,
                              uv.y - t * 1.5);
	float smoke = fbm(smokeUV * 2.5) * 0.6 + fbm(smokeUV * 5.0) * 0.3;
	float smokeUp = smoke + (1.0 - uv.y) * t; // 下ほど煙が濃い

    // ② 煙の色が現実(灰)→夢(紫)に変化
	float3 realSmoke = float3(0.55, 0.52, 0.50); // 爆弾の灰色煙
	float3 dreamSmoke = float3(0.45, 0.35, 0.75); // 夢の紫煙
	float3 smokeCol = lerp(realSmoke, dreamSmoke, t);
	smokeCol *= (0.6 + noise(uv * 8.0 + time * 0.2) * 0.4);

    // ③ 煙が旧シーンを隠す
	float smokeDensity = saturate(smokeUp * 2.0 - (1.0 - t) * 0.5);
	float3 result = lerp(oldCol.rgb, smokeCol, smokeDensity);

    // ④ 夢の光粒子（煙の中に星が見え始める）
	float dream = step(0.92, noise(uv * 30.0 + time * 0.4)) * t * smokeDensity;
	result += float3(0.85, 0.80, 1.00) * dream * 0.8;

	float alpha = 1.0 - saturate(smokeDensity * smokeDensity * 1.5);
	return float4(saturate(result), alpha);
}
