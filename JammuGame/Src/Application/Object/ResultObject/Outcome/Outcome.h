#pragma once

class Outcome :public KdGameObject
{
public:

	Outcome() { Init(); };
	~Outcome() {};

	void Init()       override;
	void DrawSprite() override;

private:


	std::shared_ptr<KdTexture> m_cleartex;
	std::shared_ptr<KdTexture> m_overtex;


};

