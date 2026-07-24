#include "GameScene.h"
#include"../SceneManager.h"

#include "../../Chikuwa/Core/ChikuwaManager.h"
#include "../../UI/UIManager.h"
#include "../../UI/ScoreDisplay/Score.h"
#include "../../GameSystem/ScoreSystem/ScoreSystem.h"
#include "../../Back/Back.h"
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
	m_chikuwa->Update();
	
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
	BaseScene::DrawSprite();
	m_chikuwa->DrawSprite();

	
	// UI
	UIManager::Instance().DrawAll();
}

void GameScene::Event()
{
	if (GetAsyncKeyState('T') & 0x8000)
	{
		SceneManager::Instance().SetNextScene
		(SceneManager::SceneType::Title);

	}
	if (GetAsyncKeyState('R') & 0x8000)
	{
		SceneManager::Instance().SetNextScene
		(SceneManager::SceneType::Result);
	}
}

void GameScene::Init()
{
	m_chikuwa = std::make_shared<ChikuwaManager>();
	m_chikuwa->Init();

	ScoreSystem::Instance().Init();

	auto score = std::make_shared<Score>();
	UIManager::Instance().Register(score);
	DIALOGUE_MGR.Register("dialogue", "factory_boss_lines");
	m_back = std::make_shared<Back>();
	m_back->Init();
	
	UIManager::Instance().Register(std::make_shared<Window>());
}

void GameScene::Enter()
{
	//BGM
	AudioManager::Instance().Play(
		L"Asset/Sounds/BGM/GameBGM.mp3",
		SoundCategory::BGM,
		1.0f,
		true);
}

void GameScene::Exit()
{
	AudioManager::Instance().StopAll(SoundCategory::BGM);
}