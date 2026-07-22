#pragma once

//外部編集用情報
struct DataCB
{
	DirectX::XMFLOAT4 color;
};

//フェード用パラメータ
struct StateParam
{
	float time = 0.0f;
	float blendFactor = 1.0f;
	float dreamPow = 0.0f;
	float padding = 0.0f;
};

//フェードタイプ
enum class TransitionType
{
	None,      // 即時切り替え
	Fade,      // クロスフェード（旧→新をブレンド）
	FadeBlack, // 黒を経由してフェード
	FadeRipple // 水紋みたいなフェード
};

constexpr UINT kScreenWidth = 1280;
constexpr UINT kScreenHeight = 720;
