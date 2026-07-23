#pragma once

class BaseScene;

class SceneManager
{
public:

	enum class SceneType
	{
		Title,
		Game
	};
	//enumをハッシュ化
	struct EnumHush {
		template <typename T>
		size_t operator()(T t) const {
			return static_cast<size_t>(t);
		}
	};

	void PreUpdate();
	void Update();
	void PostUpdate();

	void PreDraw();
	void Draw();
	void DrawSprite();
	void DrawDebug();

	// 次のシーンをセット (次のフレームから切り替わる)
	void SetNextScene(SceneType _nextScene)
	{
		m_nextSceneType = _nextScene;
	}

private:

	// マネージャーの初期化
	// インスタンス生成(アプリ起動)時にコンストラクタで自動実行
	void Init();

	// シーン切り替え関数
	void ChangeScene();

	// 現在のシーンのインスタンスを保持しているポインタ
	std::unordered_map<SceneType, std::shared_ptr<BaseScene>, EnumHush> m_sceneMap;
	std::shared_ptr<BaseScene> m_currentScene = nullptr;
	//現在のシーンの種類を保持
	SceneType m_nowSceneType = SceneType::Game;
	// 次のシーンの種類を保持している変数
	SceneType m_nextSceneType = SceneType::Game;

private:

	SceneManager();
	~SceneManager() {}

public:

	// シングルトンパターン
	// 常に存在する && 必ず1つしか存在しない(1つしか存在出来ない)
	// どこからでもアクセスが可能で便利だが
	// 何でもかんでもシングルトンという思考はNG
	static SceneManager& Instance()
	{
		static SceneManager instance;
		return instance;
	}
};
