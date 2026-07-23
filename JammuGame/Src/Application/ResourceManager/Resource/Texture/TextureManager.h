#pragma once

using json = nlohmann::json;

class TextureManager 
{
public:

	TextureManager() = default;
	~TextureManager() = default;

	TextureManager(TextureManager&) = delete;
	TextureManager& operator = (TextureManager&) = delete;

	void Init();

	std::shared_ptr<KdSquarePolygon> GetPoly(std::string _key);
	std::shared_ptr<KdTexture> GetTex(std::string _key);

private:

	void LoadData();

	void Register(std::string _key,std::shared_ptr<KdSquarePolygon> _tex);

	std::unordered_map<std::string,std::shared_ptr<KdSquarePolygon>> m_poly;
	std::unordered_map<std::string, std::shared_ptr<KdTexture>> m_texture;
	std::string m_fileName = "";
	json m_texData;
};