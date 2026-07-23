#include "ResourceManager.h"

void ResourceManager::Init()
{
	ASSET_MGR.Init();
	m_dataList = std::make_shared<JsonResourceManager>();

	m_texList = std::make_shared<TextureManager>();
	m_texList->Init();
}