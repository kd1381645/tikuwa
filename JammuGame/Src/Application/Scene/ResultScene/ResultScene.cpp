#include "ResultScene.h"
#include "../SceneManager.h"
#include"../../Object/ResultObject/Result/Result.h"
#include"../../Object/ResultObject/Outcome/Outcome.h"

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
	std::shared_ptr<Result>result;
	result = std::make_shared<Result>();
	m_objList.push_back(result);

	std::shared_ptr<Outcome>outcome;
	outcome = std::make_shared<Outcome>();
	m_objList.push_back(outcome);

}

}

void ResultScene::Enter()
{
	//BGM
	AudioManager::Instance().Play(
		L"Asset/Sounds/BGM/ResultBGM.mp3",
		SoundCategory::BGM,
		1.0f,
		true
	);
}

void ResultScene::Exit()
{
	AudioManager::Instance().StopAll(SoundCategory::BGM);
}
