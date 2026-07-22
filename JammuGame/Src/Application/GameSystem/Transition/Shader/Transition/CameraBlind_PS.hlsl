// Camera_PS_Trans_InkBleedWipe.hlsl
// 墨が右→左へ滲み広がって旧シーンを透明化
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
	float sweep = t * 1.3;

    // ① 滲みの輪郭（fbmで不規則）
	float threshold = uv.x + fbm(uv * 4.0) * 0.25 + fbm(uv * 10.0) * 0.1;
	float consumed = smoothstep(threshold + 0.06, threshold - 0.06, sweep);

    // ② 滲みの縁が墨色になる
	float bleedEdge = smoothstep(threshold + 0.12, threshold, sweep)
                    * smoothstep(threshold - 0.1, threshold, sweep);
	float3 result = lerp(oldCol.rgb, float3(0.05, 0.05, 0.08), bleedEdge * 0.7);

    // ③ 墨の飛沫
	float splat = step(0.96, hash(floor(uv * 30.0)))
                * smoothstep(0.15, 0.0, abs(sweep - threshold)) * (1.0 - consumed);
	result = lerp(result, float3(0.05, 0.05, 0.08), splat * 0.6);

	return float4(saturate(result), consumed);
}
