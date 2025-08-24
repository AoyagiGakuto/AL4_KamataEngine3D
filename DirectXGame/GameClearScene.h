#pragma once
#include "Fade.h"
#include "KamataEngine.h"

using namespace KamataEngine;

class GameClearScene {
public:
	GameClearScene() = default;
	~GameClearScene();

	void Initialize();
	void Update();
	void Draw();
	bool IsFinished() const { return finished_; }

private:
	enum class Phase { FadeIn, Main, FadeOut };
	Phase phase_ = Phase::FadeIn;

	bool finished_ = false;

	// 「GAME CLEAR」文字（任意、なければnullptr）
	Model* textModel_ = nullptr;
	WorldTransform textTransform_;

	// 背景とPressSpace（タイトルと同等）
	Model* backgroundModel_ = nullptr;
	WorldTransform backgroundTransform_;

	Model* pressSpaceModel_ = nullptr;
	WorldTransform pressSpaceTransform_;
	float blinkTimer_ = 0.0f;
	bool blinkVisible_ = true;

	Camera* camera_ = nullptr;
	Fade* fade_ = nullptr;
};
