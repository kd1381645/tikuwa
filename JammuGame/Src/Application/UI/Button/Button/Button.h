#pragma once
#include "../../Component/ButtonComponent.h"

class Button :public ButtonComponent
{
public:

	Button(std::string _key, std::function<void()> _func) : ButtonComponent(_key, _func) {};
	~Button() override = default;

private:

	//マウスがUIに乗っているときの処理
	void Hover() override;

	//マウスがUIの外にあるときの処理
	void StayOutMouse() override;

	//UIの常時更新
	void UI_Update() override;

	float m_colorValue = 1.0f;
	bool m_isPuls = true;

	float m_scaleVel = 0.0f;
	float m_brightness = 0.0f;
	float m_pulseT = 0.0f;
	float m_floatY = 0.0f;
	float m_floatVelY = 0.0f; 
};
