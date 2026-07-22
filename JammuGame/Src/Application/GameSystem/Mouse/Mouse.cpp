#include "Mouse.h"
#include "../../main.h"
#include "../../ResourceManager/ResourceManager.h"

void Mouse::Init()
{
	ResetCursorToCenter();

	m_gameMouse.x = 0.0f;
	m_gameMouse.y = 0.0f;

	m_mouseTex = RES_MGR.GetTexList()->GetTex("Cursor");
	m_reticleTex = RES_MGR.GetTexList()->GetTex("Reticle");
}

void Mouse::Update()
{
	MousePos();
	Click(m_leftClick,true);
	Click(m_rightClick,false);

	if (GetAsyncKeyState('B') & 0x8000)isDebug = true;
	if (GetAsyncKeyState('N') & 0x8000)isDebug = false;
}

void Mouse::Draw()
{
	Math::Rectangle rc = { 0,0,kImageSize,kImageSize };
	KdShaderManager::Instance().m_spriteShader.Begin();
	KdShaderManager::Instance().m_spriteShader.SetMatrix(Math::Matrix::Identity);

	if (m_isShowMouse) {
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_mouseTex, GetScreenPos().x, GetScreenPos().y, &rc);
	}
	else {
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_reticleTex, 0, 0, &rc);
	}
	KdShaderManager::Instance().m_spriteShader.End();
}

void Mouse::ResetCursorToCenter()
{
	if (isDebug)return;
	if (m_isShowMouse)return;

	HWND hwnd = Application::Instance().GetWindowHandle();
	RECT rect;
	GetWindowRect(hwnd, &rect);
	int x = static_cast<int>(rect.left + kScrWidth / 2);
	int y = static_cast<int>(rect.top + kScrHeight / 2);
	m_prevPos = { x, y };
	SetCursorPos(x, y);
}

void Mouse::MousePos()
{
	POINT currentCursorPos;
	GetCursorPos(&currentCursorPos);

	POINT clientPos = currentCursorPos;  
	ScreenToClient(Application::Instance().GetWindowHandle(), &clientPos);
	m_screenPos.x = static_cast<float>(clientPos.x) - kScrWidth /2;
	m_screenPos.y = -(static_cast<float>(clientPos.y) - kScrHeight /2);

	float deltaX = static_cast<float>(currentCursorPos.x - m_prevPos.x);
	float deltaY = -static_cast<float>(currentCursorPos.y - m_prevPos.y);  

	m_delta.x = deltaX;
	m_delta.y = deltaY;

	m_normalizedDelta.x = std::max(-1.0f, std::min(1.0f, deltaX / kMaxDelta));
	m_normalizedDelta.y = std:: max(-1.0f, std::min(1.0f, deltaY / kMaxDelta));

	m_gameMouse.x += deltaX * kSensitivity;
	m_gameMouse.y += deltaY * kSensitivity;
	
	ClampGameMousePos();

	m_mousePos.x = static_cast<long>(m_gameMouse.x);
	m_mousePos.y = static_cast<long>(m_gameMouse.y);

	ResetCursorToCenter();
}

void Mouse::Click(MouseClick& _click,bool _isLeft)
{
	_click.m_isClick = false;
	_click.m_isPush = false;

	if (_click.m_clickInterval > 0) { _click.m_clickInterval--; return; }
	_click.m_canClick = true;

	if (!_click.m_canClick) return;

	if (_isLeft) {
		if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000)){
			_click.m_isDown = true;
			return;
		}
	}
	else {
		if (!(GetAsyncKeyState(VK_RBUTTON) & 0x8000)) {
			_click.m_isDown = true;
			return;
		}
	}
	_click.m_isPush = true;
	if (!_click.m_isDown) return;

	m_clickPos = GetScreenPos();

	_click.m_isClick = true;
	_click.m_canClick = false;
	_click.m_clickInterval = _click.kIntervalTime;
	_click.m_isDown = false;
}

void Mouse::ClampGameMousePos()
{
	const float halfW = kScrWidth / 2.0f;
	const float halfH = kScrHeight / 2.0f;

	if (m_gameMouse.x < -halfW) m_gameMouse.x = -halfW;
	if (m_gameMouse.x > halfW) m_gameMouse.x = halfW;
	if (m_gameMouse.y < -halfH) m_gameMouse.y = -halfH;
	if (m_gameMouse.y > halfH) m_gameMouse.y = halfH;
}
