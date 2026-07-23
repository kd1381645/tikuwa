#pragma once

using TimeHandle = uint64_t;

class Time 
{
public:

	void UpdateTime();

	//　ゲーム内スピードの調整
	void SetTimeScale(float _timeScale) { m_timeScale = _timeScale; }

	//　コールバック設定
	void DelayCall(float _time,std::function<void()> _callback,bool _useTimeScale);

	//時間獲得
	//ゲーム起動時～今までの経過時間
	double GetTime();
	//前フレームと今フレームの差（timeScaleあり）
	double GetDeltaTime(float _coefficient = 1.0f);
	//前フレームと今フレームの差（timeScaleなし）
	double GetUnScaledDeltaTime();

	//デバッグGUI
	void renderPerformanceOverlay();

private:

	struct TimeTask 
	{
		TimeHandle handle;
		double time;
		bool useTimeScale;
		bool isActive;
		std::function<void()> callback;
	};
	std::vector<TimeTask> m_tasks = {};
	TimeHandle m_nextHandle = 1;

	float m_timeScale = 1.0f;

	std::chrono::steady_clock::time_point m_lastTime;
	std::chrono::steady_clock::time_point m_startTime;
	float m_deltaTime = 0.0f;

	//デバッグGui
	const int FRAME_HISTORY_COUNT = 100;
	std::vector<float> frameHistory;
	int historyOffset = 0; // 次にデータを書き込む位置（インデックス）

//シングルトンパターン
private:
	Time();
	~Time() = default;

	//コピーガード
	Time(const Time&) = delete;
	Time& operator=(const Time&) = delete;
	Time(Time&&) = delete;
	Time& operator=(Time&&) = delete;

public:
	static Time& Instance() {
		static Time instance;
		return instance;
	}
};