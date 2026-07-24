#include "Score.h"
#include "../../../GameSystem/ScoreSystem/ScoreSystem.h"

void Score::Init()
{
	if (!m_tex)
	{
		m_tex = std::make_shared<KdTexture>();
		m_tex->Load("Asset/Textures/Score/number.png");
	}
}

void Score::Update()
{}

void Score::Draw()
{

	// 桁数
	constexpr int digits = 6;

	int score = ScoreSystem::Instance().GetCurrentScore();

	// 数字同士の間隔
	float spacing = (TexFrameWidthX - TexFrameWidthX / 2) * m_scale.x;

	// 中央揃え
	Math::Vector2 basePos = Math::Vector2(m_basePos.x - spacing * (digits - 1) * 0.5f, m_basePos.y);

	for (int i = 0; i < digits; i++)
	{
		// 1桁づつ取り出す
		int digitIndex = (score / (int)pow(10, digits - 1 - i)) % 10;

		Math::Rectangle rect
		{
			digitIndex * TexFrameWidthX, 0, TexFrameWidthX, TexFrameWidthY
		};

		Math::Vector2 pos = basePos;

		pos.x = basePos.x + spacing * i;

		KdShaderManager::Instance().m_spriteShader.DrawTex(
			m_tex,
			pos.x, pos.y,
			TexFrameWidthX * m_scale.x, 
			TexFrameWidthY * m_scale.y,
			&rect);
	}
}

void Score::Shutdown()
{}

std::string Score::GetID() const
{
	return "Score";
}
