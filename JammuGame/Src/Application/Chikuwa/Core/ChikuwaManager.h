#pragma once

class Chikuwa;
class ChikuwaManager
{
public:

	ChikuwaManager() = default;
	~ChikuwaManager() = default;

	void Init();

	void Update();
	void PostUpdate();

	void DrawSprite();

	void Spown();

private:

	const int kSpownTime = 20;
	int m_spownTime = 0;

	const int kSpeedUpTime = 60 * 5;
	int m_speedUp = 0;
	float m_addSpeed = 1.0f;

	std::vector<std::shared_ptr<Chikuwa>> m_spChikuwaList = {};
	int m_badConditonRate = 3;

	//ボイス
	void CreateSEList();
	std::vector<std::wstring> m_chikuwaSE;
};