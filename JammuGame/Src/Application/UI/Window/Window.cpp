#include "Window.h"
#include "../../Utility/Encoding.h"
#include "../../DialogueManager/DialogueManager.h"

void Window::Init()
{
	RES_MGR.GetDataList()->Register("dialogue","di");

	m_texture = std::make_shared<KdTexture>();
	m_texture->Load(ASSET_MGR.GetFilePath("fukidashi"));

	m_text = "";
	m_scale = 1.0f;
}

void Window::Update()
{
	m_scale += (1.0f - m_scale) * 0.2f;
}

void Window::Draw()
{
	if (!m_texture) return;

	// スケールを適用した描画サイズを計算
	int currentW = static_cast<int>(m_w * m_scale);
	int currentH = static_cast<int>(m_h * m_scale);

	// ウィンドウ描画
	KdShaderManager::Instance().m_spriteShader.DrawTex(
		m_texture.get(),
		(int)m_pos.x,(int)m_pos.y,
		currentW,currentH
	);

	// セリフ文字描画
	if (!m_text.empty())
	{
		KdShaderManager::Instance().m_spriteShader.DrawFont(
			{ m_pos.x - m_w / 2 + 70,m_pos.y - m_h / 2 + 70},
			&kWhiteColor,
			"%s", m_text.c_str()
		);
	}

}

void Window::Shutdown()
{
	m_texture.reset();
}

std::string Window::GetID() const
{
	return m_id;
}

void Window::ShowLine(const std::string& key)
{
	auto data = RES_MGR.GetDataList()->Get("di", key);
	if (data.is_string())
	{
		m_text = Utf8ToSjis(data.get<std::string>());
		m_scale = 1.35f;
	}
}
