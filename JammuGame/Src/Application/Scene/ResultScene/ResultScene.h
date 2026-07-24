#pragma once

#include"../BaseScene/BaseScene.h"

class ResultScene : public BaseScene
{
public:

	ResultScene() { Init(); }
	~ResultScene() {}

private:

	void Event() override;
	void Init()  override;
	void Enter() override;
	void Exit()	 override;
	void DrawSprite()override;
};
	void DrawSprite() override;

	std::shared_ptr<KdTexture> m_tex;
	std::shared_ptr<KdTexture> m_clear;
};
