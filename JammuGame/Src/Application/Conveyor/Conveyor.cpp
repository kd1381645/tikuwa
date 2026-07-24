#include "Conveyor.h"

void Conveyor::Init()
{
	// モデルの読み込み
	// ポインタのままでは使えないため実体化
	m_tex = std::make_shared<KdTexture>();
	m_tex->Load("Asset/Textures/Conveyor/Conveyor.png");		//SetMaterial関数でテクスチャの読み込み

	// サイズ
	//Math::Matrix scaleMat = Math::Matrix::CreateScale({ -0.5, 1.1, 1 });

	// 奥行き
	//Math::Matrix transMat = Math::Matrix::CreateTranslation({ 0, 0, 0 });

	// ワールド行列の合成
	//m_mWorld = scaleMat * transMat;

	// 画像分割
	//m_polygon->SetSplit(1, 20);

	// 初期位置
	m_pos = { 0,0,0 };
}

void Conveyor::Update()
{
	// アニメーション
	int carry[20] = { 19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0 };
	//m_polygon->SetUVRect(carry[(int)m_anime]);
	m_rect = { 0,16 * carry[(int)m_anime],16,16 };
	if (m_anime <= 17)
	{
		m_anime += 0.3f;
	}
	else
	{
		m_anime = 0;
	}
}

void Conveyor::DrawSprite()
{
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_tex, m_pos.x, m_pos.y,1280,500, &m_rect);
}
