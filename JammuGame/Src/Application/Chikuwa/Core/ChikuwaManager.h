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

	std::vector<std::shared_ptr<Chikuwa>> m_spChikuwaList = {};
	int m_badConditonRate = 8;
};