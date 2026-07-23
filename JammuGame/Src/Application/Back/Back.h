#pragma once

class Back 
{
public:
	Back() = default;
	~Back() = default;

	void Init();
	void Draw();

private:

	std::shared_ptr<KdTexture> m_tex;
};