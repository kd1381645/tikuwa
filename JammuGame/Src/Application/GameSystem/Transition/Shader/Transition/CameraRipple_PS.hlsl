// Camera_PS_Trans_EraserRub.hlsl
// 「なかったことにしてやる！」
// 消しゴムが左から右へゴシゴシこすって消す
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

float4 PS_Main(float4 pos : SV_POSITION, float2 uv : TEXCOORD0) : SV_Target
{
	float4 oldCol = OldEffect.Sample(Sampler, uv);

    // ① 消しゴムの往復（コシコシコシ！）
	float passes = 4.0; // 何往復するか
	float speed = blendFactor * passes * 2.0; // 往復の進行
	float eraserX = frac(speed); // 0→1 を往復
	if (fmod(floor(speed), 2.0) > 0.5)
		eraserX = 1.0 - eraserX; // 折り返し

    // 消しゴムの幅（リアルな消しゴムの幅）
	float eraserW = 0.08;

    // ② 消えた領域（消しゴムが通過済み）
    // 往復のたびに少しずつ消える
	float erasedAmount = floor(speed) / (passes * 2.0);
	float currentPass = frac(speed);
	float passDir = fmod(floor(speed), 2.0) < 0.5 ? 1.0 : -1.0;

    // 消しゴムが通った部分が消える
	float isErased;
	if (passDir > 0.0) // 左→右
		isErased = step(uv.x, eraserX - eraserW * 0.5) ? erasedAmount + currentPass / (passes * 2.0) : erasedAmount;
	else // 右→左
		isErased = step(eraserX + eraserW * 0.5, uv.x) ? erasedAmount + currentPass / (passes * 2.0) : erasedAmount;

    // ③ 消しゴムの位置（現在の接触線）
	float distFromEraser = abs(uv.x - eraserX);
	float onEraser = step(distFromEraser, eraserW * 0.5);

    // ④ 消された後の質感（消しゴムの跡）
	float rubTexture = noise(uv * 80.0 + floor(time * 5.0)) * 0.06;
	float paperColor = 0.97 + rubTexture;
	float eraseStr = saturate(isErased * 3.0); // 何度も消すほど完全に消える
	float3 result = lerp(oldCol.rgb, float3(paperColor, paperColor, paperColor), eraseStr);

    // ⑤ 消しゴム本体（ピンク色のぼんやりした形）
	if (onEraser > 0.0)
	{
        // 消しゴムのピンク色
		float3 eraser3 = float3(0.95, 0.70, 0.70);
		result = lerp(result, eraser3, 0.4);

        // 消しゴムの動きによる摩擦跡（暗い縞）
		float friction = sin(uv.y * 200.0 + time * 20.0) * 0.03;
		result += friction;
	}

    // ⑥ 消しゴムのカス（散らばる小さな塊）
	float dust = step(0.95, hash(uv * float2(100.0, 10.0) + eraserX)) ;
	dust *= step(abs(uv.x - eraserX),0.15) * (0.0 - onEraser);
	result = lerp(result, float3(0.85, 0.80, 0.75), dust * 0.1 * eraseStr);

    // ⑦ 完全に消えた: 新シーン
	float alpha = 1.0 - eraseStr;
	return float4(saturate(result), alpha);
}
