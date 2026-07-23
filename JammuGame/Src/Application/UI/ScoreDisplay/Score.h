#pragma once
#include "../IUIBase.h"

class Score : public IUIBase
{
public:

	void Init()					override;
	void Update()				override;
	void Draw()					override;
	void Shutdown()				override;
	std::string GetID()	const	override;

private:

	std::shared_ptr<KdTexture> m_tex = nullptr;

	static constexpr int TexFrameWidthX = 64;
	static constexpr int TexFrameWidthY = 64;

	Math::Vector2 m_basePos = { 540.0f,300.0f };
};