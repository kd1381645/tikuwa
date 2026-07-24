#pragma once

class Result :public KdGameObject
{
public:

	Result() { Init(); };
	~Result() {};

	void Init()       override;
	void DrawSprite() override;

private:

	Math::Vector3 m_pos;

	std::shared_ptr<KdTexture> m_tex;

};

