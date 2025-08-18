#pragma once
#include "Fade.h"
#include "KamataEngine.h"

using namespace KamataEngine;

class TitleScene {
public:
	TitleScene();
	~TitleScene();

	void Initialize();
	void Update();
	void Draw();

	bool IsFinished() const { return finished_; }

private:
	// フェーズ
	enum class Phase { kFadeIn, kMain, kFadeOut };
	Phase phase_ = Phase::kFadeIn;

	bool finished_ = false;

	// タイトルロゴ
	Model* titleFontModel_ = nullptr;
	WorldTransform titleTransform_;
	float blinkTimer_ = 0.0f;
	bool blinkVisible_ = true;

	// 上下揺れ
	float logoMoveTimer_ = 0.0f;

	// プレイヤー表示（オブジェとして）
	Model* playerModel_ = nullptr;
	WorldTransform playerTransform_;

	Camera* camera_ = nullptr;

	// フェード（シーン内で管理）
	Fade* fade_ = nullptr;
};
