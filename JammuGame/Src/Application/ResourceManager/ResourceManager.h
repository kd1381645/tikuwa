#pragma once
#include "Resource/DataList/JsonResourceManager.h"
#include "Resource/Texture/TextureManager.h"

class ResourceManager 
{
public:

	//初期化
	void Init();

	//----------------------------------------------
	// データリストを獲得
	// Init     : 初期化
	// Resister : jsonファイルの登録(filePath,key)
	// Get      : データの獲得(key)
	//----------------------------------------------
	std::shared_ptr<JsonResourceManager>& GetDataList() { return m_dataList; }

	//----------------------------------------------
	// テクスチャリストを獲得
	// Init : 初期化
	// Get  : リソースの獲得(key)
	//----------------------------------------------
	std::shared_ptr<TextureManager>& GetTexList() { return m_texList; }

	std::string GetFilePath(const std::string& _fileName) const {return ASSET_MGR.GetFilePath(_fileName); }

private:

	//リソースの管理クラス
	std::shared_ptr<JsonResourceManager>  m_dataList;
	std::shared_ptr<TextureManager>       m_texList;

//シングルトン
private:

	ResourceManager() = default;
	~ResourceManager() = default;

	//コピーガード
	ResourceManager(const ResourceManager&) = delete;
	ResourceManager& operator = (const ResourceManager&) = delete;

public:

	static ResourceManager& GetInstance() 
	{
		static ResourceManager instance;
		return instance;
	}
};

#define RES_MGR ResourceManager::GetInstance()