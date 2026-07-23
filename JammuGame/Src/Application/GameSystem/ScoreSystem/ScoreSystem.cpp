#include "ScoreSystem.h"

void ScoreSystem::Init()
{
	RES_MGR.GetDataList()->SetOnReload(Key::Highscore, [this](const nlohmann::json& _json) {
		m_highscore.highscore = _json[Key::Highscore];
		});

	LoadSetting();
	LoadHighScore();

	m_currentScore	= 0;
	m_finalizeScore = 0;
}

void ScoreSystem::AddScore(int value)
{
	m_currentScore += value;
}

void ScoreSystem::FinalizeScore()
{
	// 時間をそのまま使わずに、係数をかける
	float timeMultiplier = Time::Instance().GetTime() * m_scoreSystemnte.timeScoreRate;
	timeMultiplier = std::clamp(timeMultiplier, m_scoreSystemnte.minTimeMultiplier, m_scoreSystemnte.maxTimeMultiplier);
	
	// 最終的なスコア確定
	m_finalizeScore = m_currentScore * timeMultiplier;

	// 最高スコア更新
	if (m_finalizeScore > m_highscore.highscore)
	{
		m_highscore.highscore = m_finalizeScore;
		SaveHighScore();
	}
}

void ScoreSystem::HighScoreReset()
{
	m_highscore.highscore = 0;
	SaveHighScore();
}

void ScoreSystem::LoadSetting()
{
	RES_MGR.GetDataList()->Register(Path::Score, Key::Score);
	m_scoreSystemnte = RES_MGR.GetDataList()->Get(Key::Score).get<_ScoreSystem>();
}

void ScoreSystem::LoadHighScore()
{
	RES_MGR.GetDataList()->Register(Path::Highscore, Key::Highscore);
	m_highscore = RES_MGR.GetDataList()->Get(Key::Highscore).get<_Highscore>();
}

void ScoreSystem::SaveHighScore()
{
	// 最高スコア取得
	json data = RES_MGR.GetDataList()->Get(Key::Highscore);
	data[Key::Highscore] = m_highscore.highscore;

	// jsonファイルを開いて書き込み
	std::ofstream output_file(Key::SaveFilePath);
	if (output_file.is_open()) {
		output_file << data.dump(4);
		output_file.close();
	}
}