#pragma once
#include "Fade.h"
#include "KamataEngine.h"
#include <string>

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

	// 背景（任意のOBJ：使わないならロード失敗してもOK）
	Model* backgroundModel_ = nullptr;
	WorldTransform backgroundTransform_;

	// 天球（確実に背景を出したい用）
	Model* skyDomeModel_ = nullptr;
	WorldTransform skyDomeWT_;

	// 「PressSpace」OBJ（点滅）
	Model* pressSpaceModel_ = nullptr;
	WorldTransform pressSpaceTransform_;
	float blinkTimer_ = 0.0f;
	bool blinkVisible_ = true;

	// ロゴ上下揺れ
	float logoMoveTimer_ = 0.0f;

	// カメラ
	Camera* camera_ = nullptr;

	// フェード（シーン内）
	Fade* fade_ = nullptr;
};
