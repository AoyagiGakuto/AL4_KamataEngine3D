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

	// ★ 追加：次に遷移すべき先
	enum class NextAction { StartGame, OpenTutorial };
	NextAction GetNextAction() const { return nextAction_; }

private:
	// フェーズ
	enum class Phase { kFadeIn, kMain, kFadeOut };
	Phase phase_ = Phase::kFadeIn;

	bool finished_ = false;

	// ==== 表示要素 ====
	// タイトルロゴ
	Model* titleFontModel_ = nullptr;
	WorldTransform titleTransform_;

	// 任意の背景OBJ（使わなくてもOK）
	Model* backgroundModel_ = nullptr;
	WorldTransform backgroundTransform_;

	// 天球（確実に背景を出す）
	Model* skyDomeModel_ = nullptr;
	WorldTransform skyDomeWT_;

	// 「PressSpace」OBJ（点滅）
	Model* pressSpaceModel_ = nullptr;
	WorldTransform pressSpaceTransform_;
	float blinkTimer_ = 0.0f;
	bool blinkVisible_ = true;

	// ★ 追加：チュートリアル誘導OBJ（例：TutorialGuide）
	Model* tutorialGuideModel_ = nullptr;
	WorldTransform tutorialGuideTransform_;

	// ロゴ上下揺れ
	float logoMoveTimer_ = 0.0f;

	// カメラ
	Camera* camera_ = nullptr;

	// フェード
	Fade* fade_ = nullptr;

	// ★ 追加：次アクション
	NextAction nextAction_ = NextAction::StartGame;
};
