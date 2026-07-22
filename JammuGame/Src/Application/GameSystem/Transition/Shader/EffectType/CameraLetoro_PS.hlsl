// Camera_PS_JPEGHell.hlsl
Texture2D SceneTexture : register(t0);
SamplerState Sampler : register(s0);
cbuffer StateParam : register(b0)
{
	float time;
	float blendFactor;
	float2 padding;
};

float hash(float2 p)
{
	return frac(sin(dot(p, float2(127.1, 311.7))) * 43758.5453);
}

float4 PS_Main(float4 pos : SV_POSITION, float2 uv : TEXCOORD0) : SV_Target
{
    // ① DCTブロック（8×8ピクセル単位の平均色）
	float2 blockSize = float2(8.0 / 1920.0, 8.0 / 1080.0);
	float2 blockUV = floor(uv / blockSize) * blockSize + blockSize * 0.5;
	float4 blockCol = SceneTexture.Sample(Sampler, blockUV);

    // ② ブロック境界でのリンギング（高周波成分のにじみ）
    // DCT係数の打ち切りによって境界にコサイン波が生じる
	float2 inBlock = frac(uv / blockSize);
	float ringX = cos(inBlock.x * 3.14159 * 8.0) * 0.04;
	float ringY = cos(inBlock.y * 3.14159 * 8.0) * 0.04;
	float4 ringed = blockCol + (ringX + ringY);

    // ③ 色度サブサンプリングのズレ（YCbCrの2x2ブロックでクロマが共有される）
	float2 chromaBlock = float2(16.0 / 1920.0, 16.0 / 1080.0);
	float2 chromaUV = floor(uv / chromaBlock) * chromaBlock + chromaBlock * 0.5;
	float4 chromaSource = SceneTexture.Sample(Sampler, chromaUV);
	float luma = dot(ringed.rgb, float3(0.299, 0.587, 0.114));
    // YCbCrで再合成（輝度は元のブロック、色度は荒い）
	ringed.r = luma + (chromaSource.r - 0.5) * 1.4;
	ringed.g = luma - (chromaSource.r - 0.5) * 0.7 - (chromaSource.b - 0.5) * 0.34;
	ringed.b = luma + (chromaSource.b - 0.5) * 1.77;

    // ④ 蚊のようなノイズ（Mosquito Noise）
    // エッジ付近で高周波ノイズが発生する
	float edgeNoise = hash(blockUV * 1000.0 + floor(time * 10.0)) * 0.12 - 0.06;
	ringed.rgb += edgeNoise;

    // ⑥ フレームが時々丸ごと崩れる（Iフレーム欠損）
	float frameGlitch = step(0.97, hash(float2(floor(time * 24.0), 0.0)));
	float lineGlitch = step(0.8, hash(float2(floor(uv.y * 50.0), floor(time * 8.0))));
	ringed.rgb += frameGlitch * lineGlitch * float3(0.3, 0, 0.3);

	return saturate(ringed);
}
