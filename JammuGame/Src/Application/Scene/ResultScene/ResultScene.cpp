#include "ResultScene.h"
#include "../SceneManager.h"
#include"../../Object/ResultObject/Result/Result.h"
#include"../../Object/ResultObject/Outcome/Outcome.h"

void ResultScene::Event()
{
	bool m_keyFlg = SceneManager::Instance().GetKeyFlg();

	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		if (m_keyFlg == false)
		{
			SceneManager::Instance().SetNextScene
			(
				SceneManager::SceneType::Title
			);
		}
		SceneManager::Instance().SetKeyFlg(true);
	}
	else
	{
		SceneManager::Instance().SetKeyFlg(false);
	}
}

void ResultScene::Init()
{
	m_tex = std::make_shared<KdTexture>();
	m_tex->Load("Asset/Textures/Result/Result.png");
	std::shared_ptr<Result>result;
	result = std::make_shared<Result>();
	m_objList.push_back(result);

	std::shared_ptr<Outcome>outcome;
	outcome = std::make_shared<Outcome>();
	m_objList.push_back(outcome);

	m_clear = std::make_shared<KdTexture>();
	m_clear->Load("Asset/Textures/Result/gameclear.png");
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

void ResultScene::DrawSprite()
{
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_tex, 0, 0, 1280, 720);
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_clear, 0, 100, 500, 250);
}
