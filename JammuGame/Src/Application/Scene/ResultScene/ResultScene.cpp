#include "ResultScene.h"
#include "../SceneManager.h"

void ResultScene::Event()
{
	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		SceneManager::Instance().SetNextScene
		(
			SceneManager::SceneType::Title
		);
	}
}

void ResultScene::Init()
{

}

void ResultScene::Enter()
{
	//BGM
	AudioManager::Instance().Play(
		L"Asset/Sounds/BGM/ResultBGM.wav",
		SoundCategory::BGM,
		0.8f,
		true
	);
}

void ResultScene::Exit()
{
	AudioManager::Instance().StopAll(SoundCategory::BGM);
}