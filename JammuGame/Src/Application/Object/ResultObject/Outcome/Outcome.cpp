#include "Outcome.h"

void Outcome::Init()
{
	m_cleartex = std::make_shared<KdTexture>();
	m_cleartex->Load("Asset/Textures/Result/gameclear.png");

	/*m_overtex = std::make_shared<KdTexture>();
	m_overtex->Load("Asset/Textures/Result/gameover.png");*/
}



void Outcome::DrawSprite()
{
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_cleartex, 0, 100, 500,250 );
	//KdShaderManager::Instance().m_spriteShader.DrawTex(m_overtex, 0, 100, 500, 250);
}

