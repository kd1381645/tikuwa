// Camera_PS_Sumie.hlsl
Texture2D SceneTexture : register(t0);
Texture2D DepthTexture : register(t1);
Texture2D NormalTexture : register(t2);
Texture2D IdTexture : register(t3);
SamplerState Sampler : register(s0);
cbuffer StateParam : register(b0)
{
	float time;
	float blendFactor;
	float dreamPow;
	float padding;
};

float hash(float2 p)
{
	return frac(sin(dot(p, float2(127.1, 311.7))) * 43758.5453);
}

float LinearizeDepth(float d)
{
	float near = 0.1;
	float far = 1000.0;
	return (2.0 * near) / (far + near - d * (far - near));
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

float3 ReconstructNormal(float2 uv, float2 texel)
{
	float c = DepthTexture.Sample(Sampler, uv).r;
	float r = DepthTexture.Sample(Sampler, uv + float2(texel.x, 0)).r;
	float l = DepthTexture.Sample(Sampler, uv + float2(-texel.x, 0)).r;
	float u = DepthTexture.Sample(Sampler, uv + float2(0, texel.y)).r;
	float d = DepthTexture.Sample(Sampler, uv + float2(0, -texel.y)).r;

    // 深度の勾配から法線を近似
	float3 normal;
	normal.x = (l - r);
	normal.y = (d - u);
	normal.z = texel.x * 2.0; // スケール調整
	return normalize(normal);
}

float Bubbles(float2 uv, float t)
{
	float acc = 0.0;
    [unroll]
	for (int i = 0; i < 3; i++)
	{
		float fi = float(i);
		float scale = 6.0 + fi * 4.0; // 大きいほど小さく数が増える
		float speed = 0.04 + fi * 0.015; // 上昇速度

		float2 guv = uv * scale;
		guv.x += sin((uv.y + t * 0.1) * 6.2831 + fi * 2.0) * 0.15; // 左右にゆらぎ
		guv.y += t * speed * scale; // 上へ流す

		float2 cell = floor(guv);
		float2 f = frac(guv);

        // 一部のセルだけ泡を出す（疎に）
		float exists = step(0.92, hash(cell + fi * 91.0 + 5.0));
		float2 center = float2(hash(cell + 11.0), hash(cell + 23.0));
		float rnd = hash(cell + fi * 37.0);

		float d = length(f - center);
		float radius = 0.10 + rnd * 0.08;

        // 細い輪っか
		float ring = smoothstep(0.025, 0.0, abs(d - radius));
        // 球の光点（少し上にずらす）
		float spot = smoothstep(0.05, 0.0, length(f - center - float2(0.03, -0.03)));

		acc += (ring + spot * 0.6) * exists;
	}
	return saturate(acc);
}

float4 PS_Main(float4 pos : SV_POSITION, float2 uv : TEXCOORD0) : SV_Target
{
	float2 texel = float2(1.0 / 1920.0, 1.0 / 1080.0);

    // ① 色ソーベル（8近傍サンプル）
	float3 s00 = SceneTexture.Sample(Sampler, uv + texel * float2(-1, -1)).rgb;
	float3 s10 = SceneTexture.Sample(Sampler, uv + texel * float2(0, -1)).rgb;
	float3 s20 = SceneTexture.Sample(Sampler, uv + texel * float2(1, -1)).rgb;
	float3 s01 = SceneTexture.Sample(Sampler, uv + texel * float2(-1, 0)).rgb;
	float3 s21 = SceneTexture.Sample(Sampler, uv + texel * float2(1, 0)).rgb;
	float3 s02 = SceneTexture.Sample(Sampler, uv + texel * float2(-1, 1)).rgb;
	float3 s12 = SceneTexture.Sample(Sampler, uv + texel * float2(0, 1)).rgb;
	float3 s22 = SceneTexture.Sample(Sampler, uv + texel * float2(1, 1)).rgb;

	float3 gxC = -s00 - 2.0 * s01 - s02 + s20 + 2.0 * s21 + s22;
	float3 gyC = -s00 - 2.0 * s10 - s20 + s02 + 2.0 * s12 + s22;
	float colorEdge = saturate((length(gxC) + length(gyC)) * 1.3);

	float sorcenter = DepthTexture.Sample(Sampler, uv).r;

	float neighbors[8];
	neighbors[0] = DepthTexture.Sample(Sampler, uv + texel * float2(-1, -1)).r;
	neighbors[1] = DepthTexture.Sample(Sampler, uv + texel * float2(0, -1)).r;
	neighbors[2] = DepthTexture.Sample(Sampler, uv + texel * float2(1, -1)).r;
	neighbors[3] = DepthTexture.Sample(Sampler, uv + texel * float2(-1, 0)).r;
	neighbors[4] = DepthTexture.Sample(Sampler, uv + texel * float2(1, 0)).r;
	neighbors[5] = DepthTexture.Sample(Sampler, uv + texel * float2(-1, 1)).r;
	neighbors[6] = DepthTexture.Sample(Sampler, uv + texel * float2(0, 1)).r;
	neighbors[7] = DepthTexture.Sample(Sampler, uv + texel * float2(1, 1)).r;

	float minDepth = sorcenter;
	float maxDepth = sorcenter;
	for (int i = 0; i < 8; i++)
	{
		minDepth = min(minDepth, neighbors[i]);
		maxDepth = max(maxDepth, neighbors[i]);
	}

// 8近傍の中で最も浅いものと最も深いものの差
	float range = maxDepth - minDepth;

	float threshold = lerp(0.01, 0.01, sorcenter);
	float depthEdge = step(threshold, range);

	// IDエッジ検出
	float centerId = IdTexture.Sample(Sampler, uv).r;

	float id0 = IdTexture.Sample(Sampler, uv + texel * float2(-1, -1)).r;
	float id1 = IdTexture.Sample(Sampler, uv + texel * float2(0, -1)).r;
	float id2 = IdTexture.Sample(Sampler, uv + texel * float2(1, -1)).r;
	float id3 = IdTexture.Sample(Sampler, uv + texel * float2(-1, 0)).r;
	float id4 = IdTexture.Sample(Sampler, uv + texel * float2(1, 0)).r;
	float id5 = IdTexture.Sample(Sampler, uv + texel * float2(-1, 1)).r;
	float id6 = IdTexture.Sample(Sampler, uv + texel * float2(0, 1)).r;
	float id7 = IdTexture.Sample(Sampler, uv + texel * float2(1, 1)).r;

// 隣ピクセルとIDが1つでも違えばエッジ
	float idEdge = 0.0;
	idEdge = max(idEdge, step(0.001, abs(centerId - id0)));
	idEdge = max(idEdge, step(0.001, abs(centerId - id1)));
	idEdge = max(idEdge, step(0.001, abs(centerId - id2)));
	idEdge = max(idEdge, step(0.001, abs(centerId - id3)));
	idEdge = max(idEdge, step(0.001, abs(centerId - id4)));
	idEdge = max(idEdge, step(0.001, abs(centerId - id5)));
	idEdge = max(idEdge, step(0.001, abs(centerId - id6)));
	idEdge = max(idEdge, step(0.001, abs(centerId - id7)));

// 未描画ピクセル（ID=-1）はエッジを出さない
	idEdge *= step(-0.5, centerId);
	
	// 法線RTから取得
	float4 ns00 = NormalTexture.Sample(Sampler, uv + texel * float2(-1, -1));
	float4 ns10 = NormalTexture.Sample(Sampler, uv + texel * float2(0, -1));
	float4 ns20 = NormalTexture.Sample(Sampler, uv + texel * float2(1, -1));
	float4 ns01 = NormalTexture.Sample(Sampler, uv + texel * float2(-1, 0));
	float4 ns21 = NormalTexture.Sample(Sampler, uv + texel * float2(1, 0));
	float4 ns02 = NormalTexture.Sample(Sampler, uv + texel * float2(-1, 1));
	float4 ns12 = NormalTexture.Sample(Sampler, uv + texel * float2(0, 1));
	float4 ns22 = NormalTexture.Sample(Sampler, uv + texel * float2(1, 1));

	float3 n00 = ns00.rgb * 2.0 - 1.0;
	float3 n10 = ns10.rgb * 2.0 - 1.0;
	float3 n20 = ns20.rgb * 2.0 - 1.0;
	float3 n01 = ns01.rgb * 2.0 - 1.0;
	float3 n21 = ns21.rgb * 2.0 - 1.0;
	float3 n02 = ns02.rgb * 2.0 - 1.0;
	float3 n12 = ns12.rgb * 2.0 - 1.0;
	float3 n22 = ns22.rgb * 2.0 - 1.0;

	float3 gxN = -n00 - 2.0 * n01 - n02 + n20 + 2.0 * n21 + n22;
	float3 gyN = -n00 - 2.0 * n10 - n20 + n02 + 2.0 * n12 + n22;
	float normalRaw = length(gxN) + length(gyN);
	float normalEdge = step(0.8, normalRaw);

	float isBackground = step(0.9999, sorcenter);
	float is3DObject = step(-0.5, centerId) * (1.0 - isBackground);
	depthEdge *= is3DObject;

	float normalEdgeStrong = step(2.0, normalRaw) * is3DObject; // ← 追加
	float normalEdgeWeak = step(0.8, normalRaw) * is3DObject; // ← 追加
	
	float contactEdge = normalEdge * (1.0 - depthEdge); // 深度エッジがない法線変化
	float boundaryEdge = max(depthEdge, normalEdge * depthEdge); // 深度エッジがある部分
	
	float finalNormalEdge = max(normalEdgeStrong, normalEdgeWeak * depthEdge);

	float edge = max(idEdge, max(depthEdge, max(finalNormalEdge, colorEdge * 0.3)));
	edge *= 10;
	
    // ⑤ 本体色
	float2 dir = uv - 0.5;
	float ca = 0.004 * (0.6 + 0.4 * sin(time * 0.8)); // 呼吸するズレ量
	float4 col;
	col.r = SceneTexture.Sample(Sampler, uv + dir * ca).r * (0.5 + dreamPow * 0.5);
	col.g = SceneTexture.Sample(Sampler, uv).g;
	col.b = SceneTexture.Sample(Sampler, uv - dir * ca).b;
	col.a = 1.0;
	
	float lum = dot(col.rgb, float3(0.299, 0.587, 0.114));
	lum = lum * 0.5;
	
	float3 paper = float3(0.92, 0.89, 0.72);
	float3 inkWash = lerp(paper * 0.95,
                          float3(0.3, 0.28, 0.32) * 0.0,
                          lum);
	float3 tinted = lerp(float3(lum, lum, lum), col.rgb, 3.0);
	inkWash = lerp(inkWash, tinted, 0.5);

    // ⑥ エッジに墨を重ねる
	float3 inkLine = float3(0.05, 0.04, 0.1);
	float3 result = lerp(inkWash, inkLine, edge * 0.6);

    // ⑦ 和紙テクスチャ
	float paperTex = (hash(uv * float2(800.0, 600.0)) - 0.5) * 0.20;
	result += paperTex;

	float2 c = (uv - 0.5) * 2.0;
	float vx = pow(abs(c.x), 2.05);
	float vy = pow(abs(c.y), 2.05);
	float mist = saturate(max(vx, vy));
	float edge_noise = noise(uv * 4.0 + time * 0.1) * 0.55;
	mist = saturate(mist + edge_noise * mist) * dreamPow * 1.2f;

	float3 mistColor = float3(0.98, 0.82, 0.75);
	
    // ⑧ 余白の美
	float2 center = uv - 0.5;
	float margin = 1.0 - saturate(length(center) * 1.5);
	float3 edgeColor = lerp(paper, mistColor, mist * blendFactor); 
	result = lerp(edgeColor * dreamPow, result, 0.6 + margin * 0.4);

	float bubble = Bubbles(uv, time) * dreamPow;
	result += bubble * 0.05;
	
	return saturate(float4(result, 1.0));
}
