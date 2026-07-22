#include "ButtonUI.h"
#include "Button/Button.h"

void ButtonUI::Init()
{
	for (auto& button : m_button)button->Init();
}

void ButtonUI::Update()
{
	for (auto& button : m_button)button->Action();
}

void ButtonUI::Draw()
{
	for (auto& button : m_button)button->Draw();
}

void ButtonUI::CreateButton(std::string _buttonName, std::function<void()> _func)
{
	m_button.push_back(std::make_shared<Button>(_buttonName,_func));
}

