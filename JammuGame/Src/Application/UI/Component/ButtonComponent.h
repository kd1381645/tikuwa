#pragma once

struct S_Button
{
	std::shared_ptr<KdTexture> m_tex = nullptr;
	DirectX::XMFLOAT2 m_texSize = {};

	Math::Rectangle m_rc = {};
	Math::Matrix m_mat = Math::Matrix::Identity;
	Math::Vector2 m_pos = Math::Vector2::Zero;
	Math::Color m_color = { 1,1,1,1 };
	DirectX::XMFLOAT2 m_texAnim = {};

	float m_scale = 1.0f;
	float kMaxScale = 1.0f;
};

class ButtonComponent
{
public:

	struct ButtonParam
	{
		int TextSizeX;
		int TextSizeY;
		int PosX;
		int PosY;
		int AnimX;
		int AnimY;
		float Scale;
		float MaxScale;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(ButtonParam, TextSizeX, TextSizeY, PosX, PosY, AnimX, AnimY, Scale, MaxScale)
	};

	ButtonComponent(std::string _key,std::function<void()> _func) :m_DataKey(_key),m_func(_func) {};
	virtual ~ButtonComponent() {};

	void Init();

	void Action();

	void Draw();

protected:

	//マウスクリックしたとの処理
	virtual void Click();

	//マウスがUIに乗っているときの処理
	virtual void Hover() {}

	//マウスがUIの外にあるときの処理
	virtual void StayOutMouse() {}

	//UIの常時更新
	virtual void UI_Update() {}

	S_Button m_data = {};
	DirectX::XMFLOAT2          m_basePos{};

private:

	//マウスがUIに乗っているかどうか
	bool OnMouse();

	void InitButton();

	void ClickAction();
	
	std::string m_DataKey = "";
	ButtonParam m_buttonParam;

	const int kClickActionTime = 10;
	int m_time = 0;
	bool isClick = false;

	std::function<void()> m_func;
};