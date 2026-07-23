#pragma once

class Mouse 
{
public:

	void Init();

	void Update();

	void Draw();

	//実際のマウスの座標
	Math::Vector2 GetVec() {
		return {
			static_cast<float>(m_mousePos.x),
			static_cast<float>(m_mousePos.y)
		};
	}

	Math::Vector2 GetScreenPos() { return m_screenPos; }

	//現在のフレームのマウスの移動量を返す
	Math::Vector2 GetDelta() { return m_delta; }

	//クリックしたスクリーン座標を返す
	Math::Vector2 GetClickPos() { return m_clickPos; }

	//押した瞬間
	bool IsClick() { return m_leftClick.m_isClick; }

	//押している間
	bool IsPush() { return m_leftClick.m_isPush; }

	bool IsRightClick() { return m_rightClick.m_isClick; }

	bool IsRightPush() { return m_rightClick.m_isPush; }

	void ResetCursorToCenter();

	Math::Vector2 GetMouseDeltaNormalized() { return m_normalizedDelta; }

	void ShowMouse(bool _isShow) { 
		m_isShowMouse = _isShow;
	};

	bool IsDebug() { return isDebug; }

private:

	void MousePos();
	void ClampGameMousePos();

	struct MouseClick {
		bool m_isClick = false;
		bool m_isPush = false;

		const int kIntervalTime = 10;
		int m_clickInterval = 0;
		bool m_canClick = true;
		bool m_isDown = false;
	};
	void Click(MouseClick& _click,bool _isLeft);

	MouseClick m_leftClick;
	MouseClick m_rightClick;

	static constexpr int kScrWidth = 1280;
	static constexpr int kScrHeight = 720;

	POINT m_mousePos = {0,0};
	POINT m_prevPos  = {0,0};

	Math::Vector2 m_gameMouse = Math::Vector2::Zero;
	Math::Vector2 m_delta = Math::Vector2::Zero;
	Math::Vector2 m_clickPos = Math::Vector2::Zero;
	Math::Vector2 m_screenPos = Math::Vector2::Zero;

	Math::Vector2 m_normalizedDelta = { 0,0 };
	const float kMaxDelta = 30.0f;

	const float kSensitivity = 1.0f;

	bool m_isShowMouse = true;
	bool isDebug = false;

	//マウス画像
	const int kImageSize = 128;
	std::shared_ptr<KdTexture> m_mouseTex;
	std::shared_ptr<KdTexture> m_reticleTex;
	float m_mouesScale = 1.0f;

//すべてのシーンでマウスをつかうためシングルトン
private:
	Mouse() = default;
	~Mouse() = default;

	Mouse(Mouse&) = delete;
	Mouse& operator = (Mouse&) = delete;
public:
	static Mouse& Instance() {
		static Mouse instance;
		return instance;
	}
};