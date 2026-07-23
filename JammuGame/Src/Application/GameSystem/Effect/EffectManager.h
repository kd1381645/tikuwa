#pragma once

struct EffectData 
{
	uint8_t _type;
	Math::Matrix m_mat;
	Math::Vector2 m_pos;
	Math::Vector2 m_move;
	int m_lifeTime;
	bool m_isActive;
	float size = 1.0f;
};

class EffectManager 
{
public:

	void Init();

	void Update();

	void Draw();

	void CreateEffect(Math::Vector2 _pos,int _num, std::string _type);

private:
	
	//エフェクト本体
	static const int kMaxEffect = 4096;
	EffectData m_effect[kMaxEffect];
	int m_nextParticle = 0;

	std::string m_type[5]
	{
		"A",
		"B",
		"C",
		"D",
		"E"
	};

	//テクスチャ
	std::vector<std::shared_ptr<KdTexture>> m_texList;
	std::unordered_map<std::string, uint8_t> m_effectType;
	


private:
	EffectManager() = default;
	~EffectManager() = default;
public:
	static EffectManager& Instance() 
	{
		static EffectManager instance;
		return instance;
	}
};