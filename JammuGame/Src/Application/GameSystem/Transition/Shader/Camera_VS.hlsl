struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
};

VS_OUT VS_Main(float2 pos : POSITION, float2 uv : TEXCOORD0)
{
	VS_OUT o;
	o.pos = float4(pos, 0.0f, 1.0f);
	o.uv = uv;
	return o;
}
