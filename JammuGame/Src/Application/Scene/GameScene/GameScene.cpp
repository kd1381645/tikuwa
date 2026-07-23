#include "GameScene.h"
#include"../SceneManager.h"

#include "../../Chikuwa/Core/ChikuwaManager.h"
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
	BaseScene::DrawSprite();
	m_chikuwa->DrawSprite();
	
	// UI
	UIManager::Instance().DrawAll();
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
	
	UIManager::Instance().Register(std::make_shared<Window>());
}
