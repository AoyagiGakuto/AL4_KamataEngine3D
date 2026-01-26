#pragma once
#include "Fade.h"
#include "KamataEngine.h"

using namespace KamataEngine;

class TutorialScene {
public:
	TutorialScene() = default;
	~TutorialScene();

	void Initialize();
	void Update();
	void Draw();
	bool IsFinished() const { return finished_; }

private:
	enum class Phase { FadeIn, Main, FadeOut };
	Phase phase_ = Phase::FadeIn;
	bool finished_ = false;

	// チュートリアル表示用 OBJ
	Model* tutorialModel_ = nullptr;
	WorldTransform tutorialTransform_;

	// 背景
	Model* backgroundModel_ = nullptr;
	WorldTransform backgroundTransform_;
	Model* skyDomeModel_ = nullptr;
	WorldTransform skyDomeWT_;

	// PressSpace
	Model* pressSpaceModel_ = nullptr;
	WorldTransform pressSpaceTransform_;
	float blinkTimer_ = 0.0f;
	bool blinkVisible_ = true;

	Camera* camera_ = nullptr;
	Fade* fade_ = nullptr;
};
