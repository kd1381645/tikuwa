#include "ChikuwaManager.h"
#include "Chikuwa.h"

void ChikuwaManager::Init()
{
	Spown();
}

void ChikuwaManager::Update()
{

	if (Mouse::Instance().IsClick()) 
	{
		Spown();
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
	if (0 <= spownRate)isSpownGood = false;
	auto newChikuwa = std::make_shared<Chikuwa>(isSpownGood);
	newChikuwa->Init();
	m_spChikuwaList.push_back(newChikuwa);

	//回帰処理
	//Time::Instance().DelayCall(3, [&]() {Spown(); }, false);
}
