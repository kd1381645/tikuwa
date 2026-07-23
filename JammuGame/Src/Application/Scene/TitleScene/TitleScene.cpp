#include "TitleScene.h"
#include "../SceneManager.h"
#include"../../Object/TitleObject/Title/Title.h"
#include"../../Object/TitleObject/Switch/Switch.h"

void TitleScene::Event()
{
	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		SceneManager::Instance().SetNextScene
		(
			SceneManager::SceneType::Game
		);
	}
}

void TitleScene::Init()
{
	std::shared_ptr<Title>title;
	title = std::make_shared<Title>();
	m_objList.push_back(title);

	std::shared_ptr<Switch> m_switch;
	m_switch = std::make_shared<Switch>();
	m_objList.push_back(m_switch);
}
