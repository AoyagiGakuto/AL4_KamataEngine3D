#pragma once
#include "KamataEngine.h"

class Fade {
public:
	enum class Status {
		None,
		FadeIn,
		FadeOut,
	};

	void Initialize();
	void Update();
	void Draw();
	void Start(Status status, float duration);
	void Stop();
	bool IsFinished() const;
	Status GetStatus() const { return status_; } // ★これを追加

private:
	uint32_t textureHandle = 0;
	KamataEngine::Sprite* sprite_ = nullptr;
	Status status_ = Status::None;
	float duration_ = 0.0f;
	float counter_ = 0.0f;
};
