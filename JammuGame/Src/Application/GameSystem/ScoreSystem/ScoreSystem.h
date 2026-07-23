#pragma once

// 基本設定
struct ScoreSystemSettingData
{
	// 時間への係数
	float timeScoreRate;

	// 時間への補正値の下限
	float minTimeMultiplier;

	// 時間への補正値の上限
	float maxTimeMultiplier;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(ScoreSystemSettingData, timeScoreRate, minTimeMultiplier, maxTimeMultiplier);
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
public:

	void Init();

	// スコアの増減
	void AddScore(int scoreValue);

	// 最終的なスコア確定
	void FinalizeScore();

	// 現在のスコア取得
	int GetCurrentScore()	const	{ return m_currentScore; }

	// 過去最高のスコア取得
	int GetHighScore()		const	{ return m_highscoreData.highscore; }

	// 最終的なスコア取得
	int GetFinalScore()		const	{ return m_finalScore; }

	// スコアリセット
	void ResetScore();

	// ハイスコアリセット
	void ResetHighScore();

private:

	// 現在のスコア
	int m_currentScore;

	// 最終的なスコア
	int m_finalScore;

	ScoreSystemSettingData	m_scoreSystemSettingData;
	HighscoreData			m_highscoreData;

private:

	// デフォルト設定読み込み
	void LoadScoreSetting();

	// 過去プレイを含めた最高スコアを読み込み
	void LoadHighScore();

	// 最高スコアを更新
	void SaveHighScore(); 

// シングルトン
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