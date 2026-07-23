#include "Window.h"

void Window::Init()
{
	m_texture = std::make_shared<KdTexture>();
	m_texture->Load(ASSET_MGR.GetFilePath("fukidashi"));
}

void Window::Update()
{

}

void Window::Draw()
{
	if (!m_texture) return;

	KdShaderManager::Instance().m_spriteShader.DrawTex(
		m_texture.get(), // シェーダリソースビューを渡す
		(int)m_pos.x,(int)m_pos.y,
		m_w,m_h
	);
}

void Window::Shutdown()
{
	m_texture.reset();
}

std::string Window::GetID() const
{
	return m_id;
}
