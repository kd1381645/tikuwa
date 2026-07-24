#pragma once
#include "../AddScore.h"

class AddScoreManager
{
public:

	void Add(const Math::Vector2& pos, int scoreValue)
	{
		auto addScore = std::make_shared<AddScore>();
		addScore->Init();
		addScore->SetPos(pos);
		addScore->SetAddScore(scoreValue);

		m_list.push_back(addScore);
	}

	void Update()
	{
		for (auto& item : m_list) item->Update();

		m_list.erase(
			std::remove_if(m_list.begin(), m_list.end(), [](const auto& item) {
				return !item->IsActive();
				}),
			m_list.end()
		);
	}

	void Draw()
	{
		KdShaderManager::Instance().m_spriteShader.Begin();
		for (auto& item : m_list) item->Draw();
		KdShaderManager::Instance().m_spriteShader.End();
		//他のとこで行列が影響しないように単位行列をセット
		KdShaderManager::Instance().m_spriteShader.SetMatrix(Math::Matrix::Identity);
	}

private:
	std::vector<std::shared_ptr<AddScore>> m_list;

// 以下シングルトン
private:

	AddScoreManager() = default;
	~AddScoreManager() = default;

	//コピーガード
	AddScoreManager(const AddScoreManager&) = delete;
	AddScoreManager& operator = (const AddScoreManager&) = delete;

public:

	static AddScoreManager& Instance()
	{
		static AddScoreManager instance;
		return instance;
	}

};