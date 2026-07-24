#include "AddScore.h"

void AddScore::Init()
{
	if (!m_tex)
	{
		m_tex = std::make_shared<KdTexture>();
		m_tex->Load("Asset/Textures/Score/number.png");
	}

	// 各種タイマー・フラグの初期化
	m_lifeTimer = m_maxLifeTimer;
	m_isActive = true;
	m_alpha = 1.0f;
	m_currentPos = m_basePos;
	m_currentScale = m_scale;
}

void AddScore::Update()
{
	if (!m_isActive) return;

	// 寿命タイマーの減算
	m_lifeTimer--;
	if (m_lifeTimer <= 0.0f)
	{
		m_isActive = false;
		return;
	}

	// 進行度
	float progress = 1.0f - (m_lifeTimer / m_maxLifeTimer);

	// 跳ね上がる位置（Y軸移動）
	float offsetY = std::sin(progress * DirectX::XM_PIDIV2) * 50.0f;
	m_currentPos.x = m_basePos.x;
	m_currentPos.y = m_basePos.y + offsetY;

	// バウンス
	float scaleImpact = 1.0f;
	if (progress < 0.2f)
	{
		scaleImpact = 1.0f + std::sin((progress / 0.2f) * DirectX::XM_PIDIV2) * 0.4f;
	}
	else
	{
		float p = (progress - 0.2f) / 0.8f;
		scaleImpact = 1.4f - (0.4f * p);
	}
	m_currentScale = m_scale * scaleImpact;

	// フェードアウト
	if (progress > 0.5f)
	{
		m_alpha = 1.0f - ((progress - 0.5f) / 0.5f);
	}
	else
	{
		m_alpha = 1.0f;
	}
}

void AddScore::Draw()
{
	if (!m_isActive) return;


	// マイナス判定と絶対値化
	bool isNegative = (m_addScore < 0);
	int score = std::abs(m_addScore);

	int digits = 0;
	int temp = score;
	do {
		digits++;
		temp /= 10;
	} while (temp > 0);

	// 演出計算済みの m_currentScale と m_currentPos を使って描画計算を行う
	float spacing = (TexFrameWidthX - TexFrameWidthX / 2) * m_currentScale.x;

	// 中央揃えの基準位置計算
	int totalChars = isNegative ? (digits + 1) : digits;
	Math::Vector2 basePos = Math::Vector2(m_currentPos.x - spacing * (totalChars - 1) * 0.5f, m_currentPos.y);

	// マイナスなら若干赤みがかかった色、プラスなら白でアルファ値を反映
	Math::Color color = isNegative
		? Math::Color(1.0f, 0.4f, 0.4f, m_alpha)
		: Math::Color(1.0f, 1.0f, 1.0f, m_alpha);

	int startIdx = 0;

	// マイナス記号の描画 ---
	if (isNegative)
	{
		int minusIndex = 10;

		Math::Rectangle rect
		{
			minusIndex * TexFrameWidthX, 0, TexFrameWidthX, TexFrameWidthY
		};

		KdShaderManager::Instance().m_spriteShader.DrawTex(
			m_tex,
			basePos.x, basePos.y,
			TexFrameWidthX * m_currentScale.x,
			TexFrameWidthY * m_currentScale.y,
			&rect,
			&color); // アルファ値（透明度）を適用

		startIdx = 1;
	}

	// 数値の描画
	for (int i = 0; i < digits; ++i)
	{
		int digitIndex = (score / (int)pow(10, digits - 1 - i)) % 10;

		Math::Rectangle rect
		{
			digitIndex * TexFrameWidthX, 0, TexFrameWidthX, TexFrameWidthY
		};

		Math::Vector2 pos;
		pos.x = basePos.x + spacing * (i + startIdx);
		pos.y = basePos.y;

		KdShaderManager::Instance().m_spriteShader.DrawTex(
			m_tex,
			pos.x, pos.y,
			TexFrameWidthX * m_currentScale.x,
			TexFrameWidthY * m_currentScale.y,
			&rect,
			&color); // アルファ値（透明度）を適用
	}
}