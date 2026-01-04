#pragma once
#include "Fade.h"
#include "KamataEngine.h"

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

	// GAME CLEAR文字
	KamataEngine::Model* textModel_ = nullptr;
	KamataEngine::WorldTransform textTransform_;
	KamataEngine::ObjectColor textColor_;

	// 背景OBJ
	KamataEngine::Model* backgroundModel_ = nullptr;
	KamataEngine::WorldTransform backgroundTransform_;

	KamataEngine::Model* skyDomeModel_ = nullptr;
	KamataEngine::WorldTransform skyDomeWT_;

	// PressSpaceOBJ
	KamataEngine::Model* pressSpaceModel_ = nullptr;
	KamataEngine::WorldTransform pressSpaceTransform_;
	KamataEngine::ObjectColor pressSpaceColor_;
	float blinkTimer_ = 0.0f;
	bool blinkVisible_ = true;

	KamataEngine::Camera* camera_ = nullptr;
	Fade* fade_ = nullptr;
};