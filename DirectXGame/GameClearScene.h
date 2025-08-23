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

	Model* textModel_ = nullptr; // "gameclear" などのOBJ
	WorldTransform textTransform_;

	Camera* camera_ = nullptr;
	Fade* fade_ = nullptr;
};
