#include "Pch.h"
#include "EffectManager.h"

void EffectManager::Init()
{
	for (int i = 0; i < 5; i++) 
	{
		m_texList.push_back(RES_MGR.GetTexList()->GetTex(m_type[i]));
		m_effectType[m_type[i]] = uint8_t(i);
	}
}

void EffectManager::Update()
{
	//エフェクト動かす
	for (int i = 0; i < kMaxEffect; i++)
	{
		m_effect[i].m_lifeTime--;
		if (m_effect[i].m_lifeTime <= 0)m_effect[i].m_isActive = false;
		m_effect[i].m_pos += m_effect[i].m_move;
		m_effect[i].size *= 0.98f;
		m_effect[i].m_mat = 
			Math::Matrix::CreateScale(m_effect[i].size) * 
			Math::Matrix::CreateTranslation(m_effect[i].m_pos.x, m_effect[i].m_pos.y,0.0f);
	}
}

void EffectManager::Draw()
{
	auto& ss = KdShaderManager::Instance().m_spriteShader;

	Math::Rectangle rec = { 0,0,16,16 };
	KdShaderManager::Instance().ChangeBlendState(KdBlendState::Add);

	//描画開始
	ss.Begin();

	for (int i = 0; i < kMaxEffect; i++)
	{
		if (!m_effect[i].m_isActive)continue;
		ss.SetMatrix(m_effect[i].m_mat);
		ss.DrawTex(m_texList[m_effect[i]._type],0,0,&rec);
	}

	ss.End();
	ss.SetMatrix(Math::Matrix::Identity);
	KdShaderManager::Instance().ChangeBlendState(KdBlendState::Alpha);
}

void EffectManager::CreateEffect(Math::Vector2 _pos, int _num, std::string _type)
{
	if (m_effectType.find(_type) == m_effectType.end())return;

	for (int i = 0; i < _num; i++) {
		m_nextParticle++;
		if (m_nextParticle >= kMaxEffect)m_nextParticle = 0;

		m_effect[m_nextParticle].m_pos = _pos;
		m_effect[m_nextParticle].m_move = { KdRandom::GetFloat(-1.0f,1.0f) * 10,KdRandom::GetFloat(-1.0f,1.0f) * 10 };
		m_effect[m_nextParticle].m_lifeTime = KdRandom::GetInt(20.0f,30.0f);
		m_effect[m_nextParticle].m_isActive = true;
		m_effect[m_nextParticle].size = 1.0f;
		m_effect[m_nextParticle]._type = m_effectType[_type];
		
		m_effect[m_nextParticle].m_mat =
			Math::Matrix::CreateScale(m_effect[m_nextParticle].size) *
			Math::Matrix::CreateTranslation(m_effect[m_nextParticle].m_pos.x, m_effect[m_nextParticle].m_pos.y, 0.0f);
	}
}
