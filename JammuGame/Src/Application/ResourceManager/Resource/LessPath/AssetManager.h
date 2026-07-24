#pragma once

class C_AssetManager
{
public:

	struct MetaData
	{
		std::string filePath;
	};

	//Assetの追加が終わったら処理を消す
	void Init();

	// Metaファイルを探してApplicationで使用するリストを作成する
	void CreateAddressablesList();

	// AddressableNameを指定してファイルパスの作成
	std::string GetFilePath(const std::string& addressableName);

	// Assetsフォルダ以下をクロールして、Metaファイルを更新していく
	void CreateMetaFileForAllFiles();

	// サポートしているファイル形式か確認する
	bool IsSupportedFile(const std::filesystem::path& filePath);

	// ファイル一つに対してのMetaファイル作成
	nlohmann::json CreateMetaFileForFile(const std::filesystem::path& srcFile);

	// AddressableName作成
	std::string AddressableName(const std::filesystem::path& srcFile) const;

	// 作成したMetaファイルを全部削除
	void DeleteAllMetaFiles();

	// 固定のファイルパスやらファイル名やら
	const std::string m_assetFilePass = "./Asset";   // Assetファイルの先頭ディレクトリ
	const std::string m_metaFileExtensionName = ".kdfwmeta"; // 作成するメタファイルの拡張子
	const std::string m_logFileName = "./Asset\\AssetManager.log";  // Log保存場所
	const std::string m_fontFilePass = "./Asset\\Textures\\Fonts"; // フォントファイルのディレクトリ(除外用)
private:
	// 対応する拡張子
	std::list<std::string> m_supportedExtensions;

	// AddressableNameとメタ情報のリスト
	std::unordered_map<std::string, MetaData> m_addressables;

	C_AssetManager() = default;
public:
	static C_AssetManager& GetInstance()
	{
		static C_AssetManager instance;
		return instance;
	}
};
#define ASSET_MGR C_AssetManager::GetInstance()