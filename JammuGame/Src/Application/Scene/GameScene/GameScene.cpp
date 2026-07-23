#include "GameScene.h"
#include"../SceneManager.h"

#include "../../Chikuwa/Core/ChikuwaManager.h"
#include "../../UI/UIManager.h"
#include "../../UI/Window/Window.h"
//#include "../../Conveyor/Conveyor.h"

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
	
	UIManager::Instance().Register(std::make_shared<Window>());

	//// コンベア追加
	//std::shared_ptr<Conveyor>conveyor;
	//// 左5個追加
	//for (float num = 0; num < 1.5; num+=0.5)
	//{
	//	conveyor = std::make_shared<Conveyor>();
	//	conveyor->SetPos({ 0 - num,0,0 });
	//	m_objList.push_back(conveyor);
	//}
	//// 右4個追加
	//for (float num = 0.5; num < 1.5; num += 0.5)
	//{
	//	conveyor = std::make_shared<Conveyor>();
	//	conveyor->SetPos({ 0 + num,0,0 });
	//	m_objList.push_back(conveyor);
	//}
}
