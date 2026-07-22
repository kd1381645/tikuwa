// Camera_PS_Trans_Pratfall.hlsl
// 転んだ瞬間のあの感覚を忠実に再現
// 効果音（ずざーっ）は各自でご用意ください
Texture2D OldEffect : register(t0);
SamplerState Sampler : register(s0);
cbuffer StateParam : register(b0)
{
	float time;
	float blendFactor;
	float2 pad;
};

float4 PS_Main(float4 pos : SV_POSITION, float2 uv : TEXCOORD0) : SV_Target
{
    // ① 転倒アニメーション
    // フェーズ1 (0→0.3): 静止→ふらつき始める
    // フェーズ2 (0.3→0.7): 急加速でズザーっと横に滑る
    // フェーズ3 (0.7→1.0): 画面外に消えていく

	float slide, tilt, squash;

	if (blendFactor < 0.2)
	{
        // ふらつき前の予兆（ぐらっとくる瞬間）
		float t = blendFactor / 0.2;
		tilt = sin(t * 3.14159) * 0.05; // 少し傾く
		slide = 0.0;
		squash = 1.0;
	}
	else if (blendFactor < 0.75)
	{
        // すっ転ぶ（加速しながら横に飛ぶ）
		float t = (blendFactor - 0.2) / 0.55;
		float accel = t * t * t; // 急加速
		slide = accel * 1.8; // 右に飛ぶ距離
		tilt = accel * 0.6; // 傾きながら
		squash = 1.0 - accel * 0.25; // 縦に縮む（衝撃で）
	}
	else
	{
        // 画面外に消えていく
		float t = (blendFactor - 0.75) / 0.25;
		slide = 1.8 + t * 1.5;
		tilt = 0.6 + t * 0.2;
		squash = 0.75;
	}

    // ② 傾き・スライドを適用したUV変換
	float2 c = uv - float2(0.5, 0.5);

    // 傾き（z軸回転）
	float cosT = cos(-tilt);
	float sinT = sin(-tilt);
	float2 rotated = float2(cosT * c.x - sinT * c.y,
                             sinT * c.x + cosT * c.y);

    // スカッシュ（縦に縮む）
	rotated.y /= squash;

    // スライド（横方向）
	float2 srcUV = rotated + float2(0.5 - slide, 0.5);

    // 画面外に出たら透明
	float alpha = step(0.0, srcUV.x) * step(srcUV.x, 1.0)
                * step(0.0, srcUV.y) * step(srcUV.y, 1.0);
	srcUV = clamp(srcUV, 0.001, 0.999);

	float4 col = OldEffect.Sample(Sampler, srcUV);

    // ③ 滑り中のモーションブラー（ずざーっ感）
	float blurStr = slide * 0.03;
	float4 blurred = col;
    [unroll]
	for (int i = 1; i <= 6; i++)
	{
		float2 blurUV = clamp(srcUV - float2(float(i) * blurStr, 0.0), 0.001, 0.999);
		blurred += OldEffect.Sample(Sampler, blurUV);
	}
	blurred /= 7.0;
	col = lerp(col, blurred, saturate(slide * 0.8));

	return float4(saturate(col.rgb), alpha);
}
