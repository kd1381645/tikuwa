#include "ScoreSystem.h"

void ScoreSystem::Init()
{
	// jsonファイルを直接更新した場合のコールバック内容を登録
	RES_MGR.GetDataList()->SetOnReload(Key::Score, [this](const nlohmann::json& _json) 
	{
		m_scoreSystemSettingData = _json.get<ScoreSystemSettingData>();
	});

	RES_MGR.GetDataList()->SetOnReload(Key::Highscore, [this](const nlohmann::json& _json)
	{
		m_highscoreData = _json.get<HighscoreData>();
	});

	LoadScoreSetting(); 
	LoadHighScore();

	m_currentScore	= 0;
	m_finalScore	= 0;
	m_isNewRecord	= false;
	m_isFinalized	= false;
}

void ScoreSystem::AddScore(int scoreValue)
{
	int previousScore = m_currentScore;

	m_currentScore += scoreValue;
	m_currentScore = std::clamp(m_currentScore, m_scoreSystemSettingData.minScore, m_scoreSystemSettingData.maxScore);

	// 実際に加算された値（カンストで削られた分を除いた差分）
	int actualAddedValue = m_currentScore - previousScore;

	// スコア変動を通知
	if (m_onScoreChanged)
	{
		m_onScoreChanged(m_currentScore, actualAddedValue);
	}
}

void ScoreSystem::FinalizeScore()
{
	// 既に確定済みなら多重実行を防ぐ
	if (m_isFinalized)
	{
		return;
	}

	// エンドレス型ゲームの為、経過時間をスコアへ適用
	// 数値が大きくなりすぎないように、経過時間へ係数をかける
	float timeMultiplier = Time::Instance().GetTime() * m_scoreSystemSettingData.timeScoreRate;
	
	// 経過時間がおかしい場合でも安全に処理する為に上限下限を設ける
	timeMultiplier = std::clamp(timeMultiplier, m_scoreSystemSettingData.minTimeMultiplier, m_scoreSystemSettingData.maxTimeMultiplier);
	
	// 最終的なスコア確定
	// timeMultiplierとの乗算結果は小数点以下切り捨てで確定する
	m_finalScore = static_cast<int>(m_currentScore * timeMultiplier);
	
	// 今回のスコア確定処理で新記録かどうかを判定し直すことを保障
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

	m_isFinalized = true;
}

void ScoreSystem::ResetScore()
{
	m_currentScore	= 0;
	m_finalScore	= 0;
	m_isNewRecord	= false;
	m_isFinalized	= false;
}

void ScoreSystem::ResetHighScore()
{
	m_highscoreData.highscore = 0;
	SaveHighScore();
}

bool ScoreSystem::PopIsNewRecord()
{
	bool result = m_isNewRecord;

	// 一回きりの処理のためフラグを消費
	m_isNewRecord = false;

	return result;
}

void ScoreSystem::LoadScoreSetting()
{
	try
	{
		RES_MGR.GetDataList()->Register(Path::Score, Key::Score);
		m_scoreSystemSettingData = RES_MGR.GetDataList()->Get(Key::Score).get<ScoreSystemSettingData>();
	}
	catch (const std::exception&)
	{
		// スコア設定用JSONが壊れていたら仮数値をセット
		m_scoreSystemSettingData.timeScoreRate		= 0.05;
		m_scoreSystemSettingData.minScore			= 0;
		m_scoreSystemSettingData.maxScore			= std::numeric_limits<int>::max() / 2;
		m_scoreSystemSettingData.minTimeMultiplier	= 1.0;
		m_scoreSystemSettingData.maxTimeMultiplier	= 100.0;
	}
}

void ScoreSystem::LoadHighScore()
{
	try
	{
		RES_MGR.GetDataList()->Register(Path::Highscore, Key::Highscore);
		m_highscoreData = RES_MGR.GetDataList()->Get(Key::Highscore).get<HighscoreData>();
	}
	catch (const std::exception&)
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
	else
	{
		assert(false && "ハイスコアの保存に失敗しました");
	}
}