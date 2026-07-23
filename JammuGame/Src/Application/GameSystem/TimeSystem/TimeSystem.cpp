#include "Pch.h"
#include "TimeSystem.h"
#include "../../main.h"

Time::Time():frameHistory(FRAME_HISTORY_COUNT, 0.0f)
{
	//タイムを初期化
	m_startTime = std::chrono::steady_clock::now();
	m_lastTime = std::chrono::steady_clock::now();
	m_timeScale = 1.0f;
}

void Time::UpdateTime()
{
	//毎フレームm_lastTimeを更新
	auto now = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed = now - m_lastTime;
	m_deltaTime = std::min(elapsed.count(), 0.1);
	m_lastTime = now;

	//コールバック処理
	for (auto& task : m_tasks)
	{
		//時間経過
		if (task.useTimeScale)
			task.time -= GetDeltaTime();
		else
			task.time -= GetUnScaledDeltaTime();

		//コールバック
		if (task.time <= 0) {
			// 時間が来たのでコールバックを実行
			task.callback();
			task.isActive = false;
		}
	}
	//コールバック削除
	m_tasks.erase(
		std::remove_if(m_tasks.begin(), m_tasks.end(), [](const auto& task)
		{ return !task.isActive; }), m_tasks.end());

}

void Time::DelayCall(float _time, std::function<void()> _callback,bool _useTimeScale)
{
	TimeTask task{};
	task.time = _time;
	task.callback = _callback;
	task.useTimeScale = _useTimeScale;
	task.isActive = true;

	m_tasks.push_back(task);
}

double Time::GetTime()
{
	//インスタンスを生成してからの経過時間
	auto now = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed = now - m_startTime;
	return elapsed.count();
}

double Time::GetDeltaTime(float _coefficient)
{
	return m_deltaTime * m_timeScale * _coefficient;
}

double Time::GetUnScaledDeltaTime()
{
	return m_deltaTime;
}

//パフォーマンスモニター
void Time::renderPerformanceOverlay() {
	ImGui::Begin(U8("パフォーマンスモニター"));

	//最大FPSを獲得
	float maxFps = Application::Instance().GetMaxFPS();

	//タイムスケールに影響しないデルタタイムを獲得
	frameHistory[historyOffset] = m_deltaTime;
	historyOffset = (historyOffset + 1) % FRAME_HISTORY_COUNT;

	// 直近のフレームタイム（秒）を取得
	float currentDt = frameHistory[(historyOffset - 1 + FRAME_HISTORY_COUNT) % FRAME_HISTORY_COUNT];

	// フレームタイムから現在のFPSを逆算
	float fps = currentDt > 0.0f ? 1.0f / currentDt : 0.0f;
	ImGui::Text("FPS: %.f (%.3f ms)", fps, currentDt * 1000.0f);

	// グラフ中央のテキスト作成
	char overlayText[32];
	std::snprintf(overlayText, sizeof(overlayText), U8("最新: %.2f ms"), currentDt * 1000.0f);

	// FPSの状態に応じてグラフの線の色を決定する
	ImVec4 plotColor;
	if (fps >= maxFps * 0.9f) {
		//９割以上なら安定
		plotColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
	}
	else if (fps >= maxFps * 0.5f) {
		//５割以上ならやや安定
		plotColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
	}
	else {
		// ５割以下は不安定
		plotColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	}

	// ImGuiのプロット用カラーを変更（PushStyleColor）
	ImGui::PushStyleColor(ImGuiCol_PlotLines, plotColor);

	// 折れ線グラフの描画
	ImGui::PlotLines(
		"Frame Time",
		frameHistory.data(),
		FRAME_HISTORY_COUNT,
		historyOffset,
		overlayText,
		0.0f,
		1.0f / maxFps, // 縦軸の最大値（0.033秒＝約30FPS）
		ImVec2(0, 80)
	);

	// 変更した色を元に戻す（PopStyleColorを忘れると他のUIの色まで変わってしまいます）
	ImGui::PopStyleColor();

	ImGui::End();
}
