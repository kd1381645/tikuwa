#pragma once
#include "IUIBase.h"

class UIManager {
public:

	// UI登録
	void Register(std::shared_ptr<IUIBase> ui) {
		const auto& id = ui->GetID();
		m_UIs[id] = ui;
		ui->Init();
	}

	// UI登録解除
	void Unregister(const std::string& id) {
		auto it = m_UIs.find(id);
		if (it != m_UIs.end()) {
			it->second->Shutdown();
			m_UIs.erase(it);
		}
	}
	//IDなしで全登録解除
	void Unregister() {
		for (auto& [id, ui] : m_UIs) ui->Shutdown();
		m_UIs.clear();
	}

	// 全UI更新・描画
	void UpdateAll(float deltaTime) {
		for (auto& [id, ui] : m_UIs) ui->Update();
	}

	void DrawAll() {
		KdShaderManager::Instance().m_spriteShader.Begin();
		for (auto& [id, ui] : m_UIs) ui->Draw();
		KdShaderManager::Instance().m_spriteShader.End();
		//他のとこで行列が影響しないように単位行列をセット
		KdShaderManager::Instance().m_spriteShader.SetMatrix(Math::Matrix::Identity);
	}

	// IDで取得（型キャスト付き）
	template<typename T>
	std::shared_ptr<T> Get(const std::string& id) {
		auto it = m_UIs.find(id);
		if (it != m_UIs.end())
			return std::dynamic_pointer_cast<T>(it->second);
		return nullptr;
	}

private:

	std::unordered_map<std::string, std::shared_ptr<IUIBase>> m_UIs;

//すべてのシーンでつかうためシングルトン
private:

	UIManager() = default;
	~UIManager() = default;

	UIManager(const UIManager&) = delete;
	UIManager& operator=(const UIManager&) = delete;

public:
	static UIManager& Instance() {
		static UIManager instance;
		return instance;
	}
};