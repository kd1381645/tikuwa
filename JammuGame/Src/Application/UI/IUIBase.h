#pragma once

class IUIBase {
public:
	virtual ~IUIBase() = default;
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Shutdown() = 0;
	virtual std::string GetID() const = 0;
};