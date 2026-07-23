#pragma once
#include "JsonResource.h"
#include "../LessPath/AssetManager.h"

namespace fs = std::filesystem;

class JsonResourceManager 
{
public:

	using json = nlohmann::json;

	// ----------------------------------------------------------
	//  コンストラクタ
	//  pollIntervalMs：監視間隔（ミリ秒）
	// ----------------------------------------------------------
	explicit JsonResourceManager(int _pollIntervalMs = 300) :
		m_isRunning(true),
		m_pollIntervalMs(_pollIntervalMs)
	{
		m_watcherThread = std::thread([this]() {WatcherLoop(); });
	}

	//コピーガード
	JsonResourceManager(const JsonResourceManager&) = delete;
	JsonResourceManager& operator = (const JsonResourceManager&) = delete;

	~JsonResourceManager() 
	{
		m_isRunning.store(false);
		m_watcherThread.join();
	}


	// ----------------------------------------------------------
	// JSON ファイルを登録する。
	// ファイルパスのみで登録する場合（ファイル名がキーになる）
	// ----------------------------------------------------------
	void Register(const std::string& filePath)
	{
		std::string path = ASSET_MGR.GetFilePath(filePath); // 存在確認も兼ねる
		Register(path, StemOf(path));
	}
	// ----------------------------------------------------------
	// JSON ファイルを登録する。
	// ファイルパスとそのキーの名前を決める場合
	// ----------------------------------------------------------
	void Register(const std::string& filePath, const std::string& key)
	{
		 // 存在確認も兼ねる
		std::lock_guard lock(m_mapMutex);
		m_resources[key] = std::make_shared<JsonResource>(ASSET_MGR.GetFilePath(filePath));
	}


	//全体のデータの取得
	json Get(const std::string& _key) const
	{
		std::lock_guard lock(m_mapMutex);
		auto it = m_resources.find(_key);
		return it != m_resources.end() ? it->second->GetAll() : json{};
	}
	//特定のデータの取得
	// ----------------------------------------------------------
	// _key   : データのキー
	// _field : フィールド
	// ----------------------------------------------------------
	json Get(const std::string& _key, const std::string& _field) const 
	{
		std::lock_guard lock(m_mapMutex);
		auto it = m_resources.find(_key);
		return it != m_resources.end() ? it->second->Get(_field) : json{};
	}


	//---------------------------------------------------------
	// IsDirty       : 指定キーのファイルが更新されているかどうか
	// ClearDirty    : 指定キーのフラグをリセット（Get()の後に呼ぶ）
	// IsAnyDirty    : ファイルが更新されているかどうか
	// ClearA;;Dirty : すべてののフラグをリセット
	//----------------------------------------------------------
	bool IsDirty(const std::string& key) const
	{
		std::lock_guard lock(m_mapMutex);
		auto it = m_resources.find(key);
		return it != m_resources.end() && it->second->IsDirty();
	}
	void ClearDirty(const std::string& key)
	{
		std::lock_guard lock(m_mapMutex);
		auto it = m_resources.find(key);
		if (it != m_resources.end()) it->second->ClearDirty();
	}
	bool IsAnyDirty() const
	{
		std::lock_guard lock(m_mapMutex);
		for (auto& [k, r] : m_resources)
			if (r->IsDirty()) return true;
		return false;
	}
	void ClearAllDirty()
	{
		std::lock_guard lock(m_mapMutex);
		for (auto& [k, r] : m_resources) r->ClearDirty();
	}
	
	//---------------------------------------------------------
	// コールバック
	// Jsonが更新された時に自動で呼ばれる関数を登録
	//----------------------------------------------------------
	void SetOnReload(const std::string& key, JsonResource::onReloadCallBack cb)
	{
		std::lock_guard lock(m_mapMutex);
		auto it = m_resources.find(key);
		if (it != m_resources.end())
			it->second->SetOnReload(std::move(cb));
	}

	//ユーティリティ
	//キーが存在するかどうか
	bool Contains(const std::string& key) const
	{
		std::lock_guard lock(m_mapMutex);
		return m_resources.contains(key);
	}
	//すべてのキーを取得
	std::vector<std::string> GetKeys() const
	{
		std::lock_guard lock(m_mapMutex);
		std::vector<std::string> keys;
		keys.reserve(m_resources.size());
		for (auto& [k, _] : m_resources) keys.push_back(k);
		return keys;
	}
	//登録しているデータ数を取得
	std::size_t Size() const
	{
		std::lock_guard lock(m_mapMutex);
		return m_resources.size();
	}

private:

	//スレッド内動作
	void WatcherLoop() 
	{
		while (m_isRunning.load()) {
			{
				std::lock_guard lock(m_mapMutex);
				for (auto& [key, resource] : m_resources)
					resource->Poll();
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(m_pollIntervalMs));
		}
	}

	//送られてくるファイルパスからパス名と拡張子をのぞく
	static std::string StemOf(const std::string& _filePath) { return fs::path(_filePath).stem().string(); }

	std::unordered_map<std::string, std::shared_ptr<JsonResource>> m_resources;
	mutable std::mutex m_mapMutex;
	std::thread        m_watcherThread;
	std::atomic<bool>  m_isRunning;
	int                m_pollIntervalMs;
};