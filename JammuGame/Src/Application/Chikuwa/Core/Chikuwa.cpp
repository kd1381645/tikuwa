#include "Pch.h"
#include "Chikuwa.h"

void Chikuwa::Init()
{
	//テクスチャ取得
	if(m_isGood)m_tex = RES_MGR.GetTexList()->GetTex("TestChikuwa");
	else m_tex = RES_MGR.GetTexList()->GetTex("TestIsobe");;
	m_pos = {-720,KdRandom::GetFloat(-80.0f,80.0f),0};
	m_isActive = true;
}

void Chikuwa::Update()
{
	if (m_chikuwaTime < kChikuwaTime)m_chikuwaTime++;
	else
	{
		m_isActive = false;
	}

	//移動処理
	if(!m_isDestroy)m_move = {KdRandom::GetFloat(1.0f, 1.2f) * m_speed,0.0f};
	else { m_move = { 6 * -m_speed,50.0f }; }
	m_pos += m_move;

	//行列作成
	m_mat = Math::Matrix::CreateScale(0.2f) *  Math::Matrix::CreateTranslation(m_pos.x,m_pos.y,0);
}

void Chikuwa::Draw()
{
	Math::Matrix defaultMat = Math::Matrix::Identity;

	//以下描画
	auto& ss = KdShaderManager::Instance().m_spriteShader;
	ss.Begin();
	ss.SetMatrix(m_mat);
	ss.DrawTex(m_tex,0,0,640,640);
	ss.End();
	//単位行列に戻す
	ss.SetMatrix(defaultMat);
}
