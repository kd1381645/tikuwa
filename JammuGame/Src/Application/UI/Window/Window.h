#pragma once
#include "../IUIBase.h"

class Window : public IUIBase
{
public:

	struct _dialogue
	{
		std::string success;
		std::string mistake;
		int tekitou;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(_dialogue, success, mistake, tekitou)
	};


	Window() {};
	~Window() {};

	void Init()		override;
	void Update()	override;
	void Draw()		override;
	void Shutdown() override;
	std::string GetID() const override;

	void ShowLine(const std::string& key);

private:

	_dialogue dialogue = {};

	std::string m_id = "Window";
	std::string m_text;

	std::shared_ptr<KdTexture> m_texture;	 // 表示する画像
	Math::Vector2 m_pos = { 0, -260 };       // 表示位置
	int m_w = 1280;							 // 表示したい幅
	int m_h = 200;							 // 表示したい高さ
};