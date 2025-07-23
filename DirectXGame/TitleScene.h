#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

class TitleScene {
public:
	TitleScene();
	~TitleScene();

	void Initialize(); // 初期化
	void Update();     // 更新
	void Draw();       // 描画

	// 外から終了判定できるようにする
	bool IsFinished() const { return finished_; }

private:
	bool finished_ = false; // シーン終了フラグ

	Model* titleFontModel_ = nullptr;
	WorldTransform titleTransform_;
	Camera* camera_ = nullptr;

	// 点滅演出用
	float blinkTimer_ = 0.0f;
	bool blinkVisible_ = true;
};
