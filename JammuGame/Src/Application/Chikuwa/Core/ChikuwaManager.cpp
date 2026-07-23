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

	if (Mouse::Instance().IsClick()) 
	{
		//ちくわはじく処理
		Math::Vector2 mousePos = Mouse::Instance().GetClickPos();
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
			hit->Destory();

			// 分割結果に応じてセリフを切り替える
			auto window = UIManager::Instance().Get<Window>("Window");
			if (window)
			{
				window->ShowLine(hit->IsGood() ? "mistake" : "success");
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
	auto newChikuwa = std::make_shared<Chikuwa>(isSpownGood);
	newChikuwa->Init();
	m_spChikuwaList.push_back(newChikuwa);

}
