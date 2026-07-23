#include "Conveyor.h"

void Conveyor::Init()
{
	// モデルの読み込み
	// ポインタのままでは使えないため実体化
	m_polygon = std::make_shared<KdSquarePolygon>();
	m_polygon->SetMaterial("Asset/Textures/Conveyor/Conveyor.png");		//SetMaterial関数でテクスチャの読み込み

	// サイズ
	Math::Matrix scaleMat = Math::Matrix::CreateScale({ -0.5, 1.2, 1 });

	// 奥行き
	Math::Matrix transMat = Math::Matrix::CreateTranslation({ 0, 0, 0 });

	// ワールド行列の合成
	m_mWorld = scaleMat * transMat;

	// 画像分割
	m_polygon->SetSplit(1, 20);

	// 初期位置
	m_pos = { 0,0,0 };
}

void Conveyor::Update()
{
	// アニメーション
	int carry[20] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19 };
	m_polygon->SetUVRect(carry[(int)m_anime]);
	if (m_anime <= 17)
	{
		++m_anime;
	}
	else
	{
		m_anime = 0;
	}
}

void Conveyor::DrawUnLit()
{
	KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_polygon, m_mWorld);
}
