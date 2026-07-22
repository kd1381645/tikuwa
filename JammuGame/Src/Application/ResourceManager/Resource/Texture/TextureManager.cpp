#include "TextureManager.h"
#include "../LessPath/AssetManager.h"
void TextureManager::Init()
{
	LoadData();
}

std::shared_ptr<KdSquarePolygon> TextureManager::GetPoly(std::string _key)
{
	auto it = m_poly.find(_key);
	if (it != m_poly.end())return it->second;
	
	assert(0 && U8("画像の存在を確認できませんでした"));
	return nullptr;
}

std::shared_ptr<KdTexture> TextureManager::GetTex(std::string _key)
{
	auto it = m_texture.find(_key);
	if (it != m_texture.end())return it->second;
	
	
	assert(0 && U8("画像の存在を確認できませんでした"));
	return nullptr;
}

void TextureManager::LoadData()
{
	//jsonからファイルパスを読み取る
	m_fileName = ASSET_MGR.GetFilePath("TextureData_js");
	std::ifstream item(m_fileName);
	if (!item.is_open())
	{
		assert(0 && U8("画像のファイルリストが開けませんでした"));
		return;
	}

	item >> m_texData;

	//jsonからモデルデータを読み取る
	m_poly.clear();
	for (auto& [key, value] : m_texData.items())
	{	
		//板ポリロード
		std::string filePath = value["fileName"];
		auto texData = std::make_shared<KdSquarePolygon>();
		texData->SetMaterial(ASSET_MGR.GetFilePath(filePath));
		Register(key, texData);

		//テクスチャロード
		m_texture[key] = std::make_shared<KdTexture>();
		m_texture[key]->Load(ASSET_MGR.GetFilePath(filePath));
	}
}

void TextureManager::Register(std::string _key, std::shared_ptr<KdSquarePolygon> _tex)
{
	m_poly[_key] = _tex;
}
