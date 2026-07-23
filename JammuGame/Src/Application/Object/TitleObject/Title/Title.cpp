#include "Title.h"

void Title::Init()
{
	m_tex = std::make_shared<KdTexture>();
	m_tex->Load("Asset/Textures/Title/Title.png");
}



void Title::DrawSprite()
{
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_tex, 0, 0, 1280,720 );
}

