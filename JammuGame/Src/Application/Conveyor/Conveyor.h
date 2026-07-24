#pragma once

class Conveyor :public KdGameObject	// すべてのゲームオブジェクトはこのクラスを継承する
{
public:
	Conveyor() { Init(); }
	~Conveyor() {};

	void Init()			override;
	void Update()		override;
	void DrawSprite()	override;

private:

	// モデルやテクスチャはポインタで管理
	std::shared_ptr<KdTexture> m_tex;

	Math::Vector2 m_pos;

	// アニメーション用の変数
	float m_anime = 0;

	//追加 岩下
	Math::Rectangle m_rect = {};

};
