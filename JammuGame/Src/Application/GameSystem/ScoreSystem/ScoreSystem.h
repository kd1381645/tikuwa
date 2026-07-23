#pragma once

// 基本設定
struct ScoreSystemSettingData
{
	// 時間への係数
	float timeScoreRate;

	// スコアの下限値（カンスト処理用）
	int minScore;

	// スコアの上限値（カンスト処理用）
	int maxScore;

	// 時間への補正値の下限
	float minTimeMultiplier;

	// 時間への補正値の上限
	float maxTimeMultiplier;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(ScoreSystemSettingData, timeScoreRate, minScore, maxScore, minTimeMultiplier, maxTimeMultiplier);
};

// 最高スコア管理用
struct HighscoreData
{
	// 最高スコア
	int highscore;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(HighscoreData, highscore)
};

namespace Key
{
	constexpr char Score[]			= "score";
	constexpr char Highscore[]		= "highscore";
}

namespace Path
{
	constexpr char Score[]			= "ScoreSystem";
	constexpr char Highscore[]		= "Highscore";
	constexpr char SaveFilePath[]	= "Asset/Data/ScoreSystem/@Highscore.json";
}

class ScoreSystem
{
// 外部処理（加算・通知・ゲッター・セッター）
public:

	void Init();

	// スコアの増減
	void AddScore(int scoreValue);

	// リザルトシーン移行時か、リザルトシーンのInitで呼ぶ
	// 最終的なスコア確定
	void FinalizeScore();

	// ゲームシーンのInitで呼ぶ
	void ResetScore();
	void ResetHighScore();

	// 数値を表示したい時に呼ぶ
	int GetCurrentScore()	const	{ return m_currentScore; }
	int GetHighScore()		const	{ return m_highscoreData.highscore; }
	int GetFinalScore()		const	{ return m_finalScore; }

	// 新記録演出を作りたいなら呼ぶ
	bool PopIsNewRecord();

	// スコア加算演出とかを作るならラムダ式を登録
	// スコア変動時処理用コールバック
	// addedValue はカンストによる補正後の実加算値
	void SetOnScoreChanged(const std::function<void(int currentScore, int addedValue)>& callback)
	{
		m_onScoreChanged = callback;
	}

	// スコア確定演出とかを作るならラムダ式を登録
	// スコア最終確定時のコールバック
	// finalScore はハイスコア判定・保存処理の完了後に通知される
	void SetOnScoreFinalized(const std::function<void(int finalScore)>& callback)
	{
		m_onScoreFinalized = callback;
	}

// 内部処理用変数
private:

	int m_currentScore;
	int m_finalScore;

	// 新記録フラグ（FinalizeScoreで立ち、PopIsNewRecordで消費される）
	bool m_isNewRecord;

	// FinalizeScore()の多重呼出防御用
	bool m_isFinalized;

	// 基本設定
	ScoreSystemSettingData	m_scoreSystemSettingData;

	// 最高スコア管理用
	HighscoreData			m_highscoreData;

	// 確定前スコア変動時処理用
	std::function<void(int currentScore, int addedValue)> m_onScoreChanged = nullptr;

	// スコアの最終確定時通知用
	std::function<void(int finalScore)> m_onScoreFinalized = nullptr;

// 内部処理（データ読み込み・保存）
private:

	// デフォルト設定読み込み
	void LoadScoreSetting();

	// 過去プレイを含めた最高スコアを読み込み
	void LoadHighScore();

	// 最高スコアを更新
	void SaveHighScore(); 

// 以下シングルトン
private:

	ScoreSystem()	= default;
	~ScoreSystem()	= default;

	//コピーガード
	ScoreSystem(const ScoreSystem&)				 = delete;
	ScoreSystem& operator = (const ScoreSystem&) = delete;

public:

	static ScoreSystem& Instance()
	{
		static ScoreSystem instance;
		return instance;
	}
};