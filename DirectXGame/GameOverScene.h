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

	// 表示
	Model* textModel_ = nullptr; // "gameover" などのOBJ（用意できなければ nullptr）
	WorldTransform textTransform_;

	Camera* camera_ = nullptr;
	Fade* fade_ = nullptr;
};
