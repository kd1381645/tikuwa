#include "TitleScene.h"
#include "../SceneManager.h"

void TitleScene::Event()
{
	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		SceneManager::Instance().SetNextScene
		(
			SceneManager::SceneType::Game
		);
	}
}

void TitleScene::Init()
{
	m_tex = std::make_shared<KdTexture>();
	m_tex->Load("Asset/Textures/Title/Title.png");

	m_Start = std::make_shared<KdTexture>();
	m_Start->Load("Asset/Textures/Title/@start.png");
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

	KdShaderManager::Instance().m_spriteShader.DrawTex(m_Start, -220, -130, 500, 250);
}
