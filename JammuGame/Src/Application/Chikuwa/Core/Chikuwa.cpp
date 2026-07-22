#include "Pch.h"
#include "Chikuwa.h"

void Chikuwa::Init()
{
	//テクスチャ取得
	m_tex = RES_MGR.GetTexList()->GetTex("TestChikuwa");
	SetPos({0,0,0});
	m_isActive = true;

	//削除処理
	Time::Instance().DelayCall(5, [&]() {m_isActive = false; }, false);
}

void Chikuwa::Update()
{
	//移動処理
	m_move += {KdRandom::GetFloat(-1.0f, 1.0f),KdRandom::GetFloat(-1.0f, 1.0f)};
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
