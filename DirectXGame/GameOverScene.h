#pragma once
#include "Fade.h"
#include "KamataEngine.h"

// 未実装です

class GameOverScene {
public:
	GameOverScene() = default;
	~GameOverScene();

	void Initialize();
	void Update();
	void Draw();
	bool IsFinished() const { return finished_; }

private:
	enum class Phase { FadeIn, Main, FadeOut };
	Phase phase_ = Phase::FadeIn;

	bool finished_ = false;

	// 「GAME OVER」文字
	KamataEngine::Model* textModel_ = nullptr;
	KamataEngine::WorldTransform textTransform_;

	// 背景OBJ
	KamataEngine::Model* backgroundModel_ = nullptr;
	KamataEngine::WorldTransform backgroundTransform_;

	KamataEngine::Model* skyDomeModel_ = nullptr;
	KamataEngine::WorldTransform skyDomeWT_;

	// 「PressSpace」OBJ（点滅）
	KamataEngine::Model* pressSpaceModel_ = nullptr;
	KamataEngine::WorldTransform pressSpaceTransform_;
	float blinkTimer_ = 0.0f;
	bool blinkVisible_ = true;

	KamataEngine::Camera* camera_ = nullptr;
	Fade* fade_ = nullptr;
};