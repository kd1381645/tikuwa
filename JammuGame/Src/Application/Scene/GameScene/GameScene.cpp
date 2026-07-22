#include "GameScene.h"
#include"../SceneManager.h"

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
}
