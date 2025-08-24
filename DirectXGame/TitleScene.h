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

	// 背景（背景OBJ）
	Model* backgroundModel_ = nullptr;
	WorldTransform backgroundTransform_;

	// 「PressSpace」OBJ（点滅表示）
	Model* pressSpaceModel_ = nullptr;
	WorldTransform pressSpaceTransform_;
	float blinkTimer_ = 0.0f;
	bool blinkVisible_ = true;

	// 上下揺れ（ロゴ）
	float logoMoveTimer_ = 0.0f;

	Camera* camera_ = nullptr;

	// フェード（シーン内で管理）
	Fade* fade_ = nullptr;
};
