#include "AssetManager.h"

// Assetの追加が終わったら処理を消す
void C_AssetManager::Init()
{
	DeleteAllMetaFiles();
	CreateMetaFileForAllFiles();
	m_addressables.clear();
	CreateAddressablesList();
}

// Metaファイルを探してAddressablesListを作成する
void C_AssetManager::CreateAddressablesList()
{
	std::ofstream log(m_logFileName);

	for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(m_assetFilePass))
	{
		// メタファイルかどうか
		if (entry.is_regular_file() && entry.path().extension().string() == m_metaFileExtensionName)
		{
			std::ifstream meta(entry.path().string());
			nlohmann::json j;
			meta >> j;

			// AddressableName
			const std::string addressable = j["AddressableName"];

			// 今回実行時のファイルパスの作成
			std::string filePath = j["SourceFile"];

			std::filesystem::path onlydi = entry.path();
			onlydi.remove_filename();
			std::string directory = onlydi.relative_path().string();
			filePath = directory + filePath;

			// メタデータから参照ファイル情報の作成
			MetaData metaData;
			metaData.filePath = filePath;
	
			// Addressableの被りは許さず
			if (m_addressables.find(addressable) != m_addressables.end())
			{
				log << "error! : " << addressable << " This AddressableName is Conflict!" << " filePath : " << metaData.filePath << std::endl;
				assert(0 && "AddressableNameが被っています！！ Logファイルを参照して下さい");
			}

			// AddressableNameをキーにしてデータを覚えておく
			m_addressables[addressable] = metaData;
		}
	}
}

// AddressableNameからファイルパス取得
std::string C_AssetManager::GetFilePath(const std::string &addressableName)
{
	if (m_addressables.find(addressableName) == m_addressables.end())
	{
		assert(0 && "指定されたAddressableNameが見つかりません！");
	}
	return m_addressables[addressableName].filePath;
}


// Assetフォルダ以下の対応ファイル全てにMetaファイルを作っていく
void C_AssetManager::CreateMetaFileForAllFiles()
{
	// log出力先
	std::ofstream log(m_logFileName);

	// 対応するAssetの拡張子を登録→最終的には外部ファイルに吐き出す
	m_supportedExtensions.clear();
	m_supportedExtensions.push_back(".json");
	m_supportedExtensions.push_back(".TTF");
	m_supportedExtensions.push_back(".png");
	m_supportedExtensions.push_back(".gltf");
	m_supportedExtensions.push_back(".wav");
	m_supportedExtensions.push_back(".mp3");
	// …more

	// 指定されたアセットフォルダ以下をクロール
	for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(m_assetFilePass))
	{
		// AssetManagerがサポートしているファイルだった
		if (entry.is_regular_file() && IsSupportedFile(entry.path()))
		{
			const std::string filePath = entry.path().string();
			// フォントファイルはFontManagerが管理するのでスキップ
			if(filePath.find(m_fontFilePass) != std::string::npos)continue;
			
			// メタファイルが有るか
			std::filesystem::path metafilePath = entry.path();
			metafilePath.replace_filename(entry.path().filename().string() + m_metaFileExtensionName);
			if (std::filesystem::exists(metafilePath) == false)
			{
				// メタファイルがなかったら新規作成
				std::ofstream metaFile(metafilePath);
				log << "CreateMetaFile!  " << metafilePath << std::endl;

				// メタファイルに書き込むデータの作成
				metaFile << CreateMetaFileForFile(entry.path());
			}
		}
	}
}

// ファイル拡張子がサポートしている形式か調べる
bool C_AssetManager::IsSupportedFile(const std::filesystem::path& filePath)
{
	for (auto& ext : m_supportedExtensions)
	{
		if (filePath.extension().string() == ext) { return true; }
	}
	return false;
}

// 渡されたファイルに対してMetaファイルを作成する
nlohmann::json C_AssetManager::CreateMetaFileForFile(const std::filesystem::path& srcFile)
{
	nlohmann::json j;
	j["SourceFile"] = srcFile.filename().string();
	j["AddressableName"] = AddressableName(srcFile);
	return j;
}

// AddressableName作成
std::string C_AssetManager::AddressableName(const std::filesystem::path& srcFile) const
{
	auto filename = srcFile.filename().string();

	// AddressableNameの指定が有るか
	auto at = filename.find("@");
	if (at != std::string::npos)
	{
		auto dot = filename.find(".");
		return filename.substr(at + 1, dot - at - 1);
	}
	// Addressableが指定されていない場合、ファイルパスをそのまま
	return srcFile.relative_path().string();
}

// Metaファイル残削除関数、めっちゃ危険
void C_AssetManager::DeleteAllMetaFiles()
{
	// Log保存場所
	std::ofstream log(m_logFileName);

	// Assetフォルダをクロール
	for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(m_assetFilePass))
	{
		// Metaファイルじゃなかったら無視
		if (entry.path().extension().string() != m_metaFileExtensionName) { continue; }

		// 削除
		if (std::filesystem::remove(entry.path()))
		{
			log << "Delete Meta File! " << entry.path().string() << std::endl;
		}
	}
}