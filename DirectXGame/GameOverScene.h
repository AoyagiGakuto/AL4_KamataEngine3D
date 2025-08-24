#pragma once
#include "Fade.h"
#include "KamataEngine.h"

using namespace KamataEngine;

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
	Model* textModel_ = nullptr;
	WorldTransform textTransform_;

	// 背景OBJ
	Model* backgroundModel_ = nullptr;
	WorldTransform backgroundTransform_;

	Model* skyDomeModel_ = nullptr;
	WorldTransform skyDomeWT_;

	// 「PressSpace」OBJ（点滅）
	Model* pressSpaceModel_ = nullptr;
	WorldTransform pressSpaceTransform_;
	float blinkTimer_ = 0.0f;
	bool blinkVisible_ = true;

	Camera* camera_ = nullptr;
	Fade* fade_ = nullptr;
};
