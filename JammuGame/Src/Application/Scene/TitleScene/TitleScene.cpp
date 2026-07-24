#include "TitleScene.h"
#include "../SceneManager.h"
#include"../../UI/UIManager.h"
#include"../../UI/Button/ButtonUI.h"

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

void TitleScene::DrawSprite()
{

	KdShaderManager::Instance().m_spriteShader.DrawTex(m_Ttex, 0, 0, 1280, 720);


	KdShaderManager::Instance().m_spriteShader.DrawTex(m_Stex, -220, -130, 500, 250);

}

void TitleScene::Init()
{
	//タイトル背景
	m_Ttex = std::make_shared<KdTexture>();
	m_Ttex->Load("Asset/Textures/Title/Title.png");

	m_Stex = std::make_shared<KdTexture>();
	m_Stex->Load("Asset/Textures/Title/@start.png");
	//auto spButton = std::make_shared<ButtonUI>();
	//spButton->CreateButton("start", [this]() {StartEvent();});
	//UIManager::Instance().Register(spButton);
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
