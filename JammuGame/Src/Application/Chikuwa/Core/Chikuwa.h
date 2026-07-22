#pragma once

class Chikuwa : KdGameObject 
{
public:

	Chikuwa(bool isGood) : m_isGood(isGood){};
	~Chikuwa() = default;

	void Init();
	void Update();
	void Draw();

	bool IsActive() { return m_isActive; }

private:

	std::weak_ptr<KdGameObject> m_wpTarget;

	std::shared_ptr<KdTexture> m_tex;
	Math::Vector2 m_pos;
	Math::Vector2 m_move;
	Math::Matrix  m_mat;

	bool m_isGood;
	bool m_isActive = true;
};