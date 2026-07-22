#pragma once
#pragma once

struct _ScoreSystem
{
	// 時間への係数
	float timeScoreRate;

	// 時間への補正値の下限
	float minTimeMultiplier;

	// 時間への補正値の上限
	float maxTimeMultiplier;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(_ScoreSystem, timeScoreRate, minTimeMultiplier, maxTimeMultiplier);
};

struct _Highscore
{
	// 最高スコア
	int highscore;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(_Highscore, highscore)
};

namespace Key
{
	constexpr char Score[]			= "score";
	constexpr char Highscore[]		= "highscore";
	constexpr char SaveFilePath[]	= "Asset/Data/ScoreSystem/@Highscore.json";
}

namespace Path
{
	constexpr char Score[]		= "ScoreSystem";
	constexpr char Highscore[]	= "Highscore";
}

class ScoreSystem
{
public:

	void Init();

	// スコアの増減
	void AddScore(int value);

	// 最終的なスコア確定
	void FinalizeScore();

	// 現在のスコア取得
	int GetScore()				const { return m_currentScore; }

	// 過去最高のスコア取得
	int GetHighScore()			const { return m_highscore.highscore; }

	// 最終的なスコア取得
	int GetFinalScore()			const { return m_finalizeScore; }

	// ハイスコアリセット
	void HighScoreReset();

private:

	// 現在のスコア
	int m_currentScore;

	// 最終的なスコア
	int m_finalizeScore;

	_ScoreSystem	m_scoreSystemnte;
	_Highscore		m_highscore;

private:

	// デフォルト設定読み込み
	void LoadSetting();

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