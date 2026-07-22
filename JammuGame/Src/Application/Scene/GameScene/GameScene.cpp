#include "GameScene.h"
#include"../SceneManager.h"
#include "../../GameSystem/ScoreSystem/ScoreSystem.h"

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


	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		ScoreSystem::Instance().AddScore(1);
	}

	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		ScoreSystem::Instance().AddScore(-1);
	}

	if (GetAsyncKeyState('P') & 0x8000)
	{
		ScoreSystem::Instance().FinalizeScore();
	}

	if (GetAsyncKeyState('O') & 0x8000)
	{
		ScoreSystem::Instance().HighScoreReset();
	}

	KdDebugGUI::Instance().ClearLog();
	KdDebugGUI::Instance().AddLog("%d\n", ScoreSystem::Instance().GetScore());
	KdDebugGUI::Instance().AddLog("%d", ScoreSystem::Instance().GetHighScore());
}

void GameScene::Init()
{
	ScoreSystem::Instance().Init();

}
