#include "ScoreSystem.h"

void ScoreSystem::Init()
{
	RES_MGR.GetDataList()->SetOnReload(Key::Score, [this](const nlohmann::json& _json) 
	{
		m_scoreSystemSettingData.timeScoreRate = _json["timeScoreRate"];
	});

	RES_MGR.GetDataList()->SetOnReload(Key::Highscore, [this](const nlohmann::json& _json)
	{
		m_highscoreData.highscore = _json[Key::Highscore];
	});

	LoadScoreSetting();
	LoadHighScore();

	m_currentScore	= 0;
	m_finalScore	= 0;
	m_isNewRecord	= false;
}

void ScoreSystem::AddScore(int scoreValue)
{
	m_currentScore += scoreValue;

	// カンスト処理
	m_currentScore = std::clamp(m_currentScore, m_scoreSystemSettingData.minScore, m_scoreSystemSettingData.maxScore);

	// スコア変動を通知
	if (m_onScoreChanged) 
	{
		m_onScoreChanged(m_currentScore, scoreValue);
	}
}

void ScoreSystem::FinalizeScore()
{
	// 時間をそのまま使わずに、係数をかける
	float timeMultiplier = Time::Instance().GetTime() * m_scoreSystemSettingData.timeScoreRate;
	timeMultiplier = std::clamp(timeMultiplier, m_scoreSystemSettingData.minTimeMultiplier, m_scoreSystemSettingData.maxTimeMultiplier);
	
	// 最終的なスコア確定
	m_finalScore = m_currentScore * timeMultiplier;

	m_isNewRecord = false;

	// 最高スコア更新
	if (m_finalScore > m_highscoreData.highscore)
	{
		m_highscoreData.highscore = m_finalScore;
		SaveHighScore();

		// 新記録通知用フラグを立てる
		m_isNewRecord = true;
	}

	if (m_onScoreFinalized)
	{
		m_onScoreFinalized(m_finalScore);
	}
}

void ScoreSystem::ResetScore()
{
	m_currentScore = 0;
	m_finalScore = 0;
}

void ScoreSystem::ResetHighScore()
{
	m_highscoreData.highscore = 0;
	SaveHighScore();
}

bool ScoreSystem::PopIsNewRecord()
{
	// フラグを保存
	bool result = m_isNewRecord;

	// 一回きりの処理のためフラグを消費
	m_isNewRecord = false;

	// 保存したフラグを返す
	return result;
}

void ScoreSystem::LoadScoreSetting()
{
	RES_MGR.GetDataList()->Register(Path::Score, Key::Score);
	m_scoreSystemSettingData = RES_MGR.GetDataList()->Get(Key::Score).get<ScoreSystemSettingData>();
}

void ScoreSystem::LoadHighScore()
{
	try
	{
		RES_MGR.GetDataList()->Register(Path::Highscore, Key::Highscore);
		m_highscoreData = RES_MGR.GetDataList()->Get(Key::Highscore).get<HighscoreData>();
	}
	catch (const std::exception& e)
	{
		// ハイスコアJSONが壊れていたら0点で復帰
		m_highscoreData.highscore = 0;

		// 正常な状態のJSONファイルを再生成する
		SaveHighScore();
	}
}

void ScoreSystem::SaveHighScore()
{
	// 最高スコア取得
	json data = RES_MGR.GetDataList()->Get(Key::Highscore);
	data[Key::Highscore] = m_highscoreData.highscore;

	// jsonファイルを開いて書き込み
	std::ofstream output_file(Path::SaveFilePath);
	if (output_file.is_open()) 
	{
		output_file << data.dump(4);
		output_file.close();
	}
}