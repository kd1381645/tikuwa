#pragma once

#include"../BaseScene/BaseScene.h"

class TitleScene : public BaseScene
{
public :

	TitleScene()  { Init(); }
	~TitleScene() {}

	void DrawSprite() override;
	//void StartEvent();


private :

	void Event() override;
	void Init()  override;

	std::shared_ptr<KdTexture> m_Ttex;
	std::shared_ptr<KdTexture> m_Stex;


};
