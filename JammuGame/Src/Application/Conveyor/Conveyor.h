#pragma once

class Conveyor :public KdGameObject	// すべてのゲームオブジェクトはこのクラスを継承する
{
public:
	Conveyor() { Init(); }
	~Conveyor() {};

	void Init() override;
	void Update() override;
	void DrawUnLit() override;

private:

	// モデルやテクスチャはポインタで管理
	std::shared_ptr<KdSquarePolygon> m_polygon;

	Math::Vector3 m_pos;

	// アニメーション用の変数
	float m_anime = 0;
};
