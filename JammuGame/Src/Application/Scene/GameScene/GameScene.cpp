#include "GameScene.h"
#include"../SceneManager.h"

#include "../../Chikuwa/Core/ChikuwaManager.h"
#include "../../Back/Back.h"
#include "../../UI/UIManager.h"
#include "../../UI/Window/Window.h"

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

	m_back = std::make_shared<Back>();
	m_back->Init();
	
	UIManager::Instance().Register(std::make_shared<Window>());
}
