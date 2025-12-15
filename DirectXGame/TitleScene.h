#pragma once
#include "Fade.h"
#include "KamataEngine.h"
#include "ScenePhase.h"

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
	ScenePhase phase_ = ScenePhase::FadeIn;

	bool finished_ = false;

	// タイトルロゴ
	KamataEngine::Model* titleFontModel_ = nullptr;
	KamataEngine::WorldTransform titleTransform_;
	float blinkTimer_ = 0.0f;
	bool blinkVisible_ = true;

	// 上下揺れ
	float logoMoveTimer_ = 0.0f;

	// プレイヤー表示
	KamataEngine::Model* playerModel_ = nullptr;
	KamataEngine::WorldTransform playerTransform_;

	KamataEngine::Camera* camera_ = nullptr;

	// フェード
	Fade* fade_ = nullptr;
};
