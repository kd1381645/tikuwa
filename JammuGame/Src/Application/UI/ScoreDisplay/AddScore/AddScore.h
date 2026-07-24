#pragma once

class AddScore
{
public:

	void Init();
	void Update();
	void Draw();

	void SetPos(const Math::Vector2 pos) { m_basePos = pos; }
	void SetScale(const Math::Vector2 scale) { m_scale = scale; }
	void SetAddScore(const int addScore) { m_addScore = addScore; }

	bool IsActive() const { return m_isActive; }

private:

	std::shared_ptr<KdTexture> m_tex = nullptr;

	static constexpr int TexFrameWidthX = 64;
	static constexpr int TexFrameWidthY = 64;

	Math::Vector2 m_basePos = { 540.0f, 300.0f };
	Math::Vector2 m_scale = { 1.0f, 1.0f };

	// 寿命タイマー (フレーム単位: 40フレームで消える設定)
	float m_lifeTimer = 40.0f;
	float m_maxLifeTimer = 40.0f;

	int m_addScore = 0;
	bool m_isActive = true;

	// --- 演出用変数 ---
	Math::Vector2 m_currentPos = { 0.0f, 0.0f };   // アニメーション適用後の位置
	Math::Vector2 m_currentScale = { 1.0f, 1.0f }; // アニメーション適用後のスケール
	float m_alpha = 1.0f;                          // アルファ値（透明度）
};