#include "Window.h"
#include "../../Utility/Encoding.h"
#include "../../DialogueManager/DialogueManager.h"

void Window::Init()
{
	RES_MGR.GetDataList()->Register("dialogue","di");
	dialogue = RES_MGR.GetDataList()->Get("di").get<_dialogue>();
	m_texture = std::make_shared<KdTexture>();
	m_texture->Load(ASSET_MGR.GetFilePath("fukidashi"));

	m_text = "";
}

void Window::Update()
{

}

void Window::Draw()
{
	if (!m_texture) return;

	// ウィンドウ描画
	KdShaderManager::Instance().m_spriteShader.DrawTex(
		m_texture.get(),
		(int)m_pos.x,(int)m_pos.y,
		m_w,m_h
	);

	// セリフ文字(吹き出し)描画
	if (!m_text.empty())
	{
		KdShaderManager::Instance().m_spriteShader.DrawFont(
			{ m_pos.x - m_w / 2 + 70,m_pos.y - m_h / 2 + 60 },
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
	std::string raw;
	if (key == "success") raw = dialogue.success;
	else if (key == "mistake") raw = dialogue.mistake;

	m_text = Utf8ToSjis(raw);
}
