#pragma once

class Switch :public KdGameObject
{
public:

	Switch() { Init(); };
	~Switch() {};

	void Init()       override;
	void DrawSprite() override;

private:

	Math::Vector3 m_pos;

	std::shared_ptr<KdTexture> m_tex;

};

