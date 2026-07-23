#include "Button.h"

void Button::Hover()
{
	constexpr float kScaleStiff = 0.20f;
	constexpr float kScaleDamp = 0.70f;

	m_scaleVel += (m_data.kMaxScale - m_data.m_scale) * kScaleStiff;
	m_scaleVel *= kScaleDamp;
	m_data.m_scale += m_scaleVel;
	m_data.m_scale = std::min(m_data.m_scale, m_data.kMaxScale);

	constexpr float kFloatTarget = -10.0f;
	constexpr float kFloatStiff = 0.15f;
	constexpr float kFloatDamp = 0.65f;

	m_floatVelY += (kFloatTarget - m_floatY) * kFloatStiff;
	m_floatVelY *= kFloatDamp;
	m_floatY += m_floatVelY;
	m_data.m_pos.y = m_basePos.y + m_floatY;

	float glowTarget = 2.0f + 0.06f * sinf(m_pulseT * 1.8f);
	m_brightness += (glowTarget - m_brightness) * 0.15f;
	m_data.m_color = { m_brightness, m_brightness, m_brightness, 1.0f };
}

void Button::StayOutMouse()
{
	constexpr float kScaleStiff = 0.028f;
	constexpr float kScaleDamp = 0.95f;

	m_scaleVel += (1.0f - m_data.m_scale) * kScaleStiff;
	m_scaleVel *= kScaleDamp;
	m_data.m_scale += m_scaleVel;
	m_data.m_scale = std::min(m_data.m_scale, m_data.kMaxScale);

	constexpr float kFloatStiff = 0.20f;
	constexpr float kFloatDamp = 0.65f;

	m_floatVelY += (0.0f - m_floatY) * kFloatStiff;
	m_floatVelY *= kFloatDamp;
	m_floatY += m_floatVelY;
	m_data.m_pos.y = m_basePos.y + m_floatY;

	m_brightness += (m_colorValue - m_brightness) * 0.12f;
	m_data.m_color = { m_brightness, m_brightness, m_brightness, 1.0f };
}

void Button::UI_Update()
{
	m_pulseT += 0.055f;
	m_colorValue = 0.75f + 0.25f * sinf(m_pulseT);  
}
