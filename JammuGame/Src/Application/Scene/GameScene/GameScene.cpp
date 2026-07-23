#include "GameScene.h"
#include"../SceneManager.h"

#include "../../Chikuwa/Core/ChikuwaManager.h"
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
}

void GameScene::Init()
{
	m_chikuwa = std::make_shared<ChikuwaManager>();
	m_chikuwa->Init();

	DIALOGUE_MGR.Register("dialogue", "factory_boss_lines");
	
	UIManager::Instance().Register(std::make_shared<Window>());
}
