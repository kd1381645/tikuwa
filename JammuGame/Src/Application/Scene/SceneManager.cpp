#include "SceneManager.h"

#include "BaseScene/BaseScene.h"
#include "TitleScene/TitleScene.h"
#include "GameScene/GameScene.h"
#include "ResultScene/ResultScene.h"

SceneManager::SceneManager()
{
	Init();
}
void SceneManager::Init()
{
	//シーンの登録
	m_sceneMap = {
		{SceneType::Title,std::make_shared<TitleScene>()},
		{SceneType::Game,std::make_shared<GameScene>()},
		{SceneType::Result,std::make_shared<ResultScene>()},
	};

	// 開始シーンに切り替え
	ChangeScene();
}

void SceneManager::PreUpdate()
{
	// シーン切替
	if (m_nextSceneType != m_nowSceneType)ChangeScene();

	m_currentScene->PreUpdate();
}

void SceneManager::Update()
{
	m_currentScene->Update();
}

void SceneManager::PostUpdate()
{
	m_currentScene->PostUpdate();
}

void SceneManager::PreDraw()
{
	m_currentScene->PreDraw();
}

void SceneManager::Draw()
{
	m_currentScene->Draw();
}

void SceneManager::DrawSprite()
{
	m_currentScene->DrawSprite();
}

void SceneManager::DrawDebug()
{
	m_currentScene->DrawDebug();
}

void SceneManager::ChangeScene()
{
	//次のシーンを作成し、現在のシーンにする
	//現在のシーンの後始末
	if (m_currentScene)m_currentScene->Exit();

	//シーンの変更＆初期化
	m_currentScene = m_sceneMap[m_nextSceneType];
	m_currentScene->Enter();

	m_nowSceneType = m_nextSceneType;
}
