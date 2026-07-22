#pragma once
#include "../IUIBase.h"

class Button;
class ButtonUI : public IUIBase
{
public:

	ButtonUI() = default;
	~ButtonUI() = default;

	void Init() override;
	void Update() override;
	void Draw() override;

	void Shutdown() override {};
	std::string GetID() const override { return "Button"; }

	void CreateButton(std::string _buttonName,std::function<void()> _func);

private:

	std::vector<std::shared_ptr<Button>> m_button;

};