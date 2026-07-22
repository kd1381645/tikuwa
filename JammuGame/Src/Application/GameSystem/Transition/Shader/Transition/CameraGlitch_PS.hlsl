Texture2D OldEffect : register(t0);
SamplerState Sampler : register(s0);

cbuffer StateParam : register(b0)
{
	float time;
	float blendFactor;
	float2 pad;
};

// ─────────────────────────────────────────
// ノイズ系（参考コードに合わせた構成）
// ─────────────────────────────────────────
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
	return noise(p) * 0.500
         + noise(p * 2.1 + 3.7) * 0.250
         + noise(p * 4.3) * 0.125;
}

// ─────────────────────────────────────────
// ピクセルシェーダー
// ─────────────────────────────────────────
float4 PS_Main(float4 pos : SV_POSITION, float2 uv : TEXCOORD0) : SV_Target
{
	float4 oldCol = OldEffect.Sample(Sampler, uv);
	float t = blendFactor; // 0.0（開始）→ 1.0（完全移行）

    // ── ① 中央原点・アスペクト補正 ──
	float2 centered = uv * 2.0 - 1.0;
	centered.x *= 16.0 / 9.0;
	float dist = length(centered);
	float angle = atan2(centered.y, centered.x);

    // ── ② 雲前線の半径（tが進むにつれ外へ広がる） ──
	float radius = t * 2.6;

    // ── ③ fBmで縁をもこもこにする ──
	float2 noiseUV = float2(cos(angle), sin(angle)) * 3.8
                   + float2(time * 0.12, -time * 0.09);
	float bump = fbm(noiseUV * 2.5 + time * 0.1) * 0.05 ;

	float edge = radius + bump + (sin(time * 0.1) * sin(time * 0.1) * 0.3);
	float edgeW = 0.40;

    // ── ④ 内側・外側の判定 ──
	float alpha = smoothstep(edge - edgeW, edge + edgeW * 0.5, dist);

    // ── ⑤ 縁の白いフリンジ（もこもこの雲感） ──
	float fringe = 1.0 - smoothstep(edge - edgeW * 0.5, edge + edgeW * 1.5, dist);
	fringe *= smoothstep(edge - edgeW * 2.5, edge - edgeW * 0.5, dist);
	fringe *= smoothstep(0.0, 0.15, t); // 開幕直後は出さない

    // ── ⑥ 雲の縁に参考コードの煙色を乗せる ──
	float3 realSmoke = float3(0.95, 0.90, 0.90);
	float3 dreamSmoke = float3(1.00, 0.95, 0.95);
	float3 smokeCol = lerp(realSmoke, dreamSmoke, t);
	smokeCol *= 0.6 + noise(uv * 8.0 + time * 0.2) * 0.4; // 細かいムラ

    // ── ⑦ 夢の光粒子（内側に星が見え始める） ──
	float dream = step(0.92, noise(uv * 30.0 + time * 0.4))
                * saturate(t * 2.0 - 0.3) // t>0.5 から出現
                * (1.0 - alpha); // 内側のみ
	float3 dreamColor = oldCol.rgb + float3(0.85, 0.80, 1.00) * dream * 0.8;

    // ── ⑧ 合成 ──
	float3 result = lerp(dreamColor, smokeCol, fringe);
	result = lerp(result, oldCol.rgb, alpha);

	return float4(saturate(result), alpha + saturate(fringe));
}
