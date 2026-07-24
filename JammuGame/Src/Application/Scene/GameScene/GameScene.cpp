#include "GameScene.h"
#include"../SceneManager.h"

#include "../../Chikuwa/Core/ChikuwaManager.h"
#include "../../UI/UIManager.h"
#include "../../UI/ScoreDisplay/CurrentScore/Score.h"
#include "../../UI/ScoreDisplay/AddScore/Manager/AddScoreManager.h"
#include "../../GameSystem/ScoreSystem/ScoreSystem.h"

void GameScene::Update()
{	
	BaseScene::Update();
	m_chikuwa->Update();
	AddScoreManager::Instance().Update();
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

	AddScoreManager::Instance().Draw();
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
	ScoreSystem::Instance().Init();

	m_chikuwa = std::make_shared<ChikuwaManager>();
	m_chikuwa->Init();

	auto score = std::make_shared<Score>();
	UIManager::Instance().Register(score);
}
