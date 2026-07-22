// Camera_PS_Trans_RealityUnravel.hlsl
// 部屋の壁紙が端からほどけるように
// ひらひらと剥がれ落ちて夢の世界が現れる
// ゲームのエンディング感にも最適
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
	float a = frac(sin(dot(i, float2(127.1, 311.7))) * 43758.5);
	float b = frac(sin(dot(i + float2(1, 0), float2(127.1, 311.7))) * 43758.5);
	float c = frac(sin(dot(i + float2(0, 1), float2(127.1, 311.7))) * 43758.5);
	float d = frac(sin(dot(i + float2(1, 1), float2(127.1, 311.7))) * 43758.5);
	return lerp(lerp(a, b, f.x), lerp(c, d, f.x), f.y);
}
float fbm(float2 p)
{
	return noise(p) * .5 + noise(p * 2.1 + 3.7) * .25 + noise(p * 4.3) * .125;
}

float4 PS_Main(float4 pos : SV_POSITION, float2 uv : TEXCOORD0) : SV_Target
{
	float4 oldCol = OldEffect.Sample(Sampler, uv);
	float t = blendFactor;

    // ① 現実がほどける（端から内側へ波状に）
    // fbmで不規則な剥がれ方
	float peelFront = fbm(uv * 3.0 + float2(t * 0.5, 0.0)) * 0.3;
	float peelDist = max(abs(uv.x - 0.5), abs(uv.y - 0.5)) * 2.0; // 端からの距離
	float isPeeled = step(peelDist + peelFront, t * 1.5);

    // ② 剥がれる縁のカール（紙が捲れる感じ）
	float curlEdge = abs(peelDist + peelFront - t * 1.5);
	float curl = smoothstep(0.08, 0.0, curlEdge);

    // 捲れた紙の裏（少し明るい）
	float3 paperBack = float3(0.92, 0.90, 0.95);
	float3 result = lerp(oldCol.rgb, paperBack, curl * 0.6);

    // ③ 剥がれた後: 夢の世界が見える
	float3 dreamWorld = float3(0.12, 0.08, 0.28); // 深い夢の暗さ
    // 夢の中に浮かぶ光の粒
	float dreamLight = fbm(uv * 4.0 + time * 0.06 + float2(5.0, 2.0));
	float3 dreamColor = lerp(dreamWorld,
                              float3(0.50, 0.40, 0.80),
                              dreamLight * isPeeled);
    // 夢の星
	float dreamStars = step(0.95, noise(uv * 50.0 + time * 0.3)) * isPeeled;
	dreamColor += float3(0.85, 0.80, 1.00) * dreamStars * 0.6;

	result = lerp(result, dreamColor, isPeeled * 0.85);

    // ④ 縁の光（剥がれる瞬間のきらめき）
	result += float3(0.90, 0.85, 1.00) * curl * (1.0 - isPeeled) * 0.6;

	float alpha = 1.0 - isPeeled * isPeeled;
	return float4(saturate(result), alpha);
}
