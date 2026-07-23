#pragma once
#include "../IUIBase.h"

class Window : public IUIBase
{
public:

	Window() {};
	~Window() {};

	void Init()		override;
	void Update()	override;
	void Draw()		override;
	void Shutdown() override;
	std::string GetID() const override;

private:
	std::string m_id = "Window";

	std::shared_ptr<KdTexture> m_texture;	 // 表示する画像
	Math::Vector2 m_pos = { 0, -260 };       // 表示位置
	int m_w = 1280;							 // 表示したい幅
	int m_h = 200;							 // 表示したい高さ
};