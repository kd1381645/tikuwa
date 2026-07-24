#pragma once

#include"../Scene/BaseScene/BaseScene.h"

class ResultScene : public BaseScene
{
public :

	ResultScene()  { Init(); }
	~ResultScene() {}

private :

	void Event() override;
	void Init()  override;
};
