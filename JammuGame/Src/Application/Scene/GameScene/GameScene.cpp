#include "GameScene.h"
#include"../SceneManager.h"

#include "../../Chikuwa/Core/ChikuwaManager.h"
#include "../../Back/Back.h"

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
	m_back->Draw();
	BaseScene::DrawSprite();
	m_chikuwa->DrawSprite();
}

void GameScene::Event()
{
	if (GetAsyncKeyState('T') & 0x8000)
	{
		SceneManager::Instance().SetNextScene
		(SceneManager::SceneType::Title);

	}
}

void GameScene::Init()
{
	m_chikuwa = std::make_shared<ChikuwaManager>();
	m_chikuwa->Init();

	m_back = std::make_shared<Back>();
	m_back->Init();
}
