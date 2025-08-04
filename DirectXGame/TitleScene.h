#pragma once
#include "KamataEngine.h"
#include "Fade.h"

using namespace KamataEngine;

class TitleScene {
public:
	enum class Phase {
		kFadeIn,
		kMain,
		kFadeOut,
	};

	TitleScene();
	~TitleScene();

	void Initialize();
	void Update();
	void Draw();

	bool IsFinished() const { return finished_; }
	bool IsFadeFinished() const { return phase_ == Phase::kFadeOut && fade_->IsFinished(); }

private:
	bool finished_ = false;

	// タイトルロゴ
	Model* titleFontModel_ = nullptr;
	WorldTransform titleTransform_;
	float blinkTimer_ = 0.0f;
	bool blinkVisible_ = true;

	// 上下揺れ用タイマー
	float logoMoveTimer_ = 0.0f;

	// プレイヤー表示
	Model* playerModel_ = nullptr;
	WorldTransform playerTransform_;

	Camera* camera_ = nullptr;

	Fade* fade_ = nullptr;
	Phase phase_ = Phase::kFadeIn;
};
