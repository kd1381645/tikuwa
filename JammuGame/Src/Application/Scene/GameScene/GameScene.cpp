#include "GameScene.h"
#include"../SceneManager.h"

#include "../../Chikuwa/Core/ChikuwaManager.h"
#include "../../UI/UIManager.h"
#include "../../UI/ScoreDisplay/CurrentScore/Score.h"
#include "../../UI/ScoreDisplay/AddScore/Manager/AddScoreManager.h"
#include "../../GameSystem/ScoreSystem/ScoreSystem.h"
#include "../../Back/Back.h"
#include "../../Conveyor/Conveyor.h"
#include "../../UI/UIManager.h"
#include "../../UI/Window/Window.h"
#include "../../DialogueManager/DialogueManager.h"

GameScene::~GameScene()
{
	UIManager::Instance().Unregister();
}

void GameScene::Update()
{	
	BaseScene::Update();
	m_conveyor->Update();
	m_chikuwa->Update();
	AddScoreManager::Instance().Update();
	
	// UI
	UIManager::Instance().UpdateAll(0);
}

void GameScene::PostUpdate()
{
	BaseScene::PostUpdate();
	m_chikuwa->PostUpdate();
}

void GameScene::DrawSprite()
{
	m_back->Draw();
	m_conveyor->DrawSprite();
	BaseScene::DrawSprite();
	EffectManager::Instance().Draw();
	m_chikuwa->DrawSprite();
	AddScoreManager::Instance().Draw();
	

	// UI
	UIManager::Instance().DrawAll();
}

void GameScene::Event()
{
	if (GetAsyncKeyState('R') & 0x8000)
	{
		SceneManager::Instance().SetNextScene
		(SceneManager::SceneType::Result);

	}
	if (GetAsyncKeyState('R') & 0x8000)
	{
		SceneManager::Instance().SetNextScene
		(SceneManager::SceneType::Result);
	}
}

void GameScene::Init()
{
	ScoreSystem::Instance().Init();

	m_chikuwa = std::make_shared<ChikuwaManager>();
	m_chikuwa->Init();


	auto score = std::make_shared<Score>();
	score->SetColor(kWhiteColor);
	UIManager::Instance().Register(score);
	DIALOGUE_MGR.Register("dialogue", "factory_boss_lines");
	m_back = std::make_shared<Back>();
	m_back->Init();

	m_conveyor = std::make_shared<Conveyor>();
	
	UIManager::Instance().Register(std::make_shared<Window>());
}

void GameScene::Enter()
{
	//BGM
	AudioManager::Instance().Play(
		L"Asset/Sounds/BGM/GameBGM.wav",
		SoundCategory::BGM,
		0.8f,
		true);
}

void GameScene::Exit()
{
	UIManager::Instance().Unregister();
	AudioManager::Instance().StopAll(SoundCategory::BGM);
}