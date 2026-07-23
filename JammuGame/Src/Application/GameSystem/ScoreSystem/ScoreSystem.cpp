#include "ScoreSystem.h"

void ScoreSystem::Init()
{
	RES_MGR.GetDataList()->SetOnReload(Key::Score, [this](const nlohmann::json& _json) {
		m_scoreSystemSettingData.timeScoreRate = _json["timeScoreRate"];
		});

	RES_MGR.GetDataList()->SetOnReload(Key::Highscore, [this](const nlohmann::json& _json) {
		m_highscoreData.highscore = _json[Key::Highscore];
		});

	LoadScoreSetting();
	LoadHighScore();

	m_currentScore	= 0;
	m_finalScore	= 0;
}

void ScoreSystem::AddScore(int scoreValue)
{
	m_currentScore += scoreValue;
}

void ScoreSystem::FinalizeScore()
{
	// 時間をそのまま使わずに、係数をかける
	float timeMultiplier = Time::Instance().GetTime() * m_scoreSystemSettingData.timeScoreRate;
	timeMultiplier = std::clamp(timeMultiplier, m_scoreSystemSettingData.minTimeMultiplier, m_scoreSystemSettingData.maxTimeMultiplier);
	
	// 最終的なスコア確定
	m_finalScore = m_currentScore * timeMultiplier;

	// 最高スコア更新
	if (m_finalScore > m_highscoreData.highscore)
	{
		m_highscoreData.highscore = m_finalScore;
		SaveHighScore();
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

void ScoreSystem::LoadScoreSetting()
{
	RES_MGR.GetDataList()->Register(Path::Score, Key::Score);
	m_scoreSystemSettingData = RES_MGR.GetDataList()->Get(Key::Score).get<ScoreSystemSettingData>();
}

void ScoreSystem::LoadHighScore()
{
	RES_MGR.GetDataList()->Register(Path::Highscore, Key::Highscore);
	m_highscoreData = RES_MGR.GetDataList()->Get(Key::Highscore).get<HighscoreData>();
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