#include "ChikuwaManager.h"
#include "Chikuwa.h"
#include "../../UI/UIManager.h"
#include "../../UI/Window/Window.h"

void ChikuwaManager::Init()
{
}

void ChikuwaManager::Update()
{
	if (m_spownTime < kSpownTime)m_spownTime++;
	else
	{
		Spown();
		m_spownTime = KdRandom::GetInt(0,kSpownTime - 1);
	}

	if (m_speedUp < kSpeedUpTime)m_speedUp++;
	else 
	{
		if(m_addSpeed < 3.0f)m_addSpeed *= 1.01f;
		m_speedUp = 0;
	}

	if (Mouse::Instance().IsClick()) 
	{
		Math::Vector2 mousePos = Mouse::Instance().GetClickPos();
		EffectManager::Instance().CreateEffect(mousePos,10,"A");

		//ちくわはじく処理
		std::shared_ptr<Chikuwa> hit = nullptr;
		float minLength = 0.0f;
		for(auto chikuwa : m_spChikuwaList)
		{
			Math::Vector2 chikuwaPos = chikuwa->GetPos();
			Math::Vector2 distance = chikuwaPos - mousePos;
			if (abs(distance.x) <= 128 / 2&&
				abs(distance.y) <= 128 / 2)
			{
				float length = distance.Length();
				if (!hit) {
					hit = chikuwa;
					minLength = length;
					continue;
				}

				if (length > minLength)continue;
				hit = chikuwa;
				minLength = length;
			};
		}
		if (hit) 
		{
			//SE
			if (hit->IsGood())
			{
				AudioManager::Instance().Play(
					L"Asset/Sounds/SE/Fail.mp3",
					SoundCategory::SE,
					0.3f);
			}
			else
			{
				AudioManager::Instance().Play(
					L"Asset/Sounds/SE/Success.mp3",
					SoundCategory::SE,
					0.8f);
			}

			hit->Destory();

			// 分割結果に応じてセリフを切り替える
			auto window = UIManager::Instance().Get<Window>("Window");
			if (window)
			{
				window->ShowLine(hit->IsGood() ? "mistake" : hit->GetTypeName());
			}
		}
	}

	//更新処理
	for (auto chikuwa : m_spChikuwaList) 
	{
		chikuwa->Update();
	}
}

void ChikuwaManager::PostUpdate()
{
	//いらないちくわ消す
	m_spChikuwaList.erase(
		std::remove_if(m_spChikuwaList.begin(), m_spChikuwaList.end(),
			[this](const auto& chikuwa) {
				if (!chikuwa->IsActive())return true;
				return false;
			}),
		m_spChikuwaList.end());
}

void ChikuwaManager::DrawSprite()
{
	//描画処理
	for (auto chikuwa : m_spChikuwaList)
	{
		chikuwa->Draw();
	}
}

void ChikuwaManager::Spown()
{
	//生成確率設定
	int spownRate = KdRandom::GetInt(0, m_badConditonRate);

	//生成
	bool isSpownGood = true;
	if (0 >= spownRate)isSpownGood = false;
	auto newChikuwa = std::make_shared<Chikuwa>(isSpownGood,m_addSpeed);
	newChikuwa->Init();
	m_spChikuwaList.push_back(newChikuwa);
}
