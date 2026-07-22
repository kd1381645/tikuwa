#include "GameScene.h"
#include"../SceneManager.h"

#include "../../Chikuwa/Core/ChikuwaManager.h"

void GameScene::Update()
{
	BaseScene::Update();
	m_chikuwa->Update();
}

void GameScene::PostUpdate()
{
	BaseScene::PostUpdate();
	m_chikuwa->PostUpdate();
}

void GameScene::DrawSprite()
{
	BaseScene::DrawSprite();
	m_chikuwa->DrawSprite();
}

void GameScene::Event()
{
	if (GetAsyncKeyState('T') & 0x8000)
	{
		CameraEffect::Instance().StartWipeTransition(
			"overlay", 0.5f,
			[this]() {
				SceneManager::Instance().SetNextScene
				(SceneManager::SceneType::Title);
			},
			"sumie");
	}
}

void GameScene::Init()
{
	m_chikuwa = std::make_shared<ChikuwaManager>();
	m_chikuwa->Init();
}
