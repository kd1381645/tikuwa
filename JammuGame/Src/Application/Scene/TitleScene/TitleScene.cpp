#include "TitleScene.h"
#include "../SceneManager.h"
#include"../../UI/UIManager.h"
#include"../../UI/Button/ButtonUI.h"

void TitleScene::Event()
{
	bool m_keyFlg = SceneManager::Instance().GetKeyFlg();

	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		if (m_keyFlg == false)
		{
			SceneManager::Instance().SetNextScene
			(
				SceneManager::SceneType::Game
			);
		}
		SceneManager::Instance().SetKeyFlg(true);
	}
	else
	{
		SceneManager::Instance().SetKeyFlg(false);
	}
}

void TitleScene::Init()
{
	m_tex = std::make_shared<KdTexture>();
	m_tex->Load("Asset/Textures/Title/Title.png");
	m_start = std::make_shared<KdTexture>();
	m_start->Load("Asset/Textures/Title/@start.png");

}

void TitleScene::Enter()
{
	//BGM
	AudioManager::Instance().Play(
		L"Asset/Sounds/BGM/TitleBGM.mp3",
		SoundCategory::BGM,
		1.0f,
		true);
}

void TitleScene::Exit()
{
	AudioManager::Instance().StopAll(SoundCategory::BGM);
}

void TitleScene::DrawSprite()
{
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_tex, 0, 0, 1280, 720);

	KdShaderManager::Instance().m_spriteShader.DrawTex(m_start, -220, -130, 500, 250);
}
