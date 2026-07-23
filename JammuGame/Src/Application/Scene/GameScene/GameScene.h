#pragma once

#include"../BaseScene/BaseScene.h"

class Back;
class ChikuwaManager;

class GameScene : public BaseScene
{
public :

	GameScene()  { Init(); }
	~GameScene() {}

	void Update() override;
	void PostUpdate() override;

	void DrawSprite() override;

private:

	void Event() override;
	void Init()  override;

	std::shared_ptr<ChikuwaManager> m_chikuwa;
	std::shared_ptr<Back> m_back;
};
