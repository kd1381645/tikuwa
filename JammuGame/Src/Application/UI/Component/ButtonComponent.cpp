#include "ButtonComponent.h"
#include "../../ResourceManager/ResourceManager.h"
#include "../../GameSystem/Mouse/Mouse.h"

void ButtonComponent::Init()
{
	RES_MGR.GetDataList()->Register("ButtonData_js","Button");
	m_buttonParam = RES_MGR.GetDataList()->Get("Button",m_DataKey).get<ButtonParam>();
	RES_MGR.GetDataList()->SetOnReload("Button", [this](const nlohmann::json& json) {
		m_buttonParam = json[m_DataKey].get<ButtonParam>();
		InitButton();
		RES_MGR.GetDataList()->ClearDirty("Button");
		});

	InitButton();
}

void ButtonComponent::Action()
{
	if (OnMouse()) {
		if (Mouse::Instance().IsClick())Click();
		Hover();
	}
	else StayOutMouse();

	UI_Update();

	ClickAction();

	m_data.m_mat =
		Math::Matrix::CreateScale(m_data.m_scale) * 
		Math::Matrix::CreateTranslation(m_data.m_pos.x, m_data.m_pos.y,0);
}

void ButtonComponent::Draw()
{
	m_data.m_rc = {
		(int)(m_data.m_texSize.x * m_data.m_texAnim.x),
		(int)(m_data.m_texSize.y * m_data.m_texAnim.y),
		(int)(m_data.m_texSize.x),(int)(m_data.m_texSize.y) };
	KdShaderManager::Instance().m_spriteShader.SetMatrix(m_data.m_mat);
	KdShaderManager::Instance().m_spriteShader.DrawTex(
		m_data.m_tex, 0, 0, &m_data.m_rc, &m_data.m_color, {0.5f,0.5f});
}

void ButtonComponent::Click()
{
	KdAudioManager::Instance().Play(RES_MGR.GetFilePath("ButtonClick_SE"));
	isClick = true;
}

bool ButtonComponent::OnMouse()
{	
	Math::Vector2 mousePos = Mouse::Instance().GetScreenPos();
	Math::Vector2 buttonPos = Math::Vector2(m_data.m_pos.x, m_data.m_pos.y);
	Math::Vector2 distance = buttonPos - mousePos;
	if (abs(distance.x) <= m_data.m_texSize.x / 2 * m_data.kMaxScale &&
		abs(distance.y) <= m_data.m_texSize.y / 2 * m_data.kMaxScale)
	{
		return true;
	}
	return false;	
}

void ButtonComponent::InitButton()
{
	m_data.m_texSize =
	{ (float)m_buttonParam.TextSizeX,(float)m_buttonParam.TextSizeY };
	m_data.m_pos =
	{ (float)m_buttonParam.PosX,(float)m_buttonParam.PosY };
	m_data.m_texAnim =
	{ (float)m_buttonParam.AnimX,(float)m_buttonParam.AnimY };
	m_data.kMaxScale = m_buttonParam.MaxScale;
	m_data.m_scale = m_buttonParam.Scale;
	m_basePos = m_data.m_pos;

	m_data.m_tex = RES_MGR.GetTexList()->GetTex(m_DataKey);
}

void ButtonComponent::ClickAction()
{
	if (!isClick)return;
	if (m_time < kClickActionTime) 
	{
		m_time++;
		m_data.m_color = { 0.5f, 0.5f, 0.5f, 1.0f };
	}
	else m_func();
}
