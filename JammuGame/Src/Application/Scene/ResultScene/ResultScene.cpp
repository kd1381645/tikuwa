#include "ResultScene.h"
#include "../SceneManager.h"
#include"../../Object/ResultObject/Result/Result.h"
#include"../../Object/ResultObject/Outcome/Outcome.h"
#include "../../GameSystem/ScoreSystem/ScoreSystem.h"
#include "../../UI/ScoreDisplay/CurrentScore/Score.h"
#include "../../UI/UIManager.h"

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
	ScoreSystem::Instance().FinalizeScore();
	auto score = std::make_shared<Score>();
	score->SetPos({ 0.0f,-100.0f });
	score->SetScale({ 3.0f,3.0f });
	score->SetCurrent(false);
	score->SetColor({0.8f,0.1f,0.1f,1});
	UIManager::Instance().Register(score);
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

void ResultScene::DrawSprite()
{
	BaseScene::DrawSprite();
	UIManager::Instance().DrawAll();
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_tex, 0, 0, 1280, 720);
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_clear, 0, 100, 500, 250);
}
