#include "Pch.h"
#include "Back.h"

void Back::Init()
{
	m_tex = RES_MGR.GetTexList()->GetTex("Back");
}

void Back::Draw()
{
	auto& ss = KdShaderManager::Instance().m_spriteShader;
	ss.Begin();
	ss.DrawTex(m_tex, 0, 0, 1280, 720);
	ss.End();
}
