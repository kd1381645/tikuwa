#include "GameScene.h"
#include"../SceneManager.h"

#include "../../Chikuwa/Core/ChikuwaManager.h"
#include "../../UI/UIManager.h"
#include "../../UI/ScoreDisplay/Score.h"
#include "../../GameSystem/ScoreSystem/ScoreSystem.h"

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

	UIManager::Instance().DrawAll();
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

	ScoreSystem::Instance().Init();

	auto score = std::make_shared<Score>();
	UIManager::Instance().Register(score);
}
