#include "TitleScene.h"
#include "../SceneManager.h"

void TitleScene::Event()
{
	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		SceneManager::Instance().SetNextScene
		(
			SceneManager::SceneType::Game
		);
	}
}

void TitleScene::Init()
{
}

void TitleScene::Enter()
{
	//BGM
	AudioManager::Instance().Play(
		L"Asset/Sounds/BGM/TitleBGM.mp3",
		SoundCategory::BGM,
		1.0f,
		true);
}

void TitleScene::Exit()
{
	AudioManager::Instance().StopAll(SoundCategory::BGM);
}