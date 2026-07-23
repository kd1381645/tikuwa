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
	bool IsGood() { return m_isGood; }

	void Destory() { m_isDestroy = true; }

	const Math::Vector2& GetPos(){ return m_pos; }

private:

	const int kChikuwaTime = 5 * 60;
	int m_chikuwaTime = 0;

	std::weak_ptr<KdGameObject> m_wpTarget;

	std::shared_ptr<KdTexture> m_tex;
	Math::Vector2 m_pos;
	Math::Vector2 m_move;
	Math::Matrix  m_mat;

	float m_speed = 10.0f; 

	bool m_isGood;
	bool m_isActive = true;
	bool m_isDestroy = false;
};