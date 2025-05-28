#pragma once
#include "KamataEngine.h"
#include "Player.h"

using namespace KamataEngine;
using namespace MathUtility;

class CameraController {
public:
	void Initialize();
	void Update();
	void SetTarget(Player* target) { target_ = target; };
	void Reset();

private:
	// カメラ
	Camera camera_;
	Player* target_ = nullptr;
	Vector3 targetOffset_ = {0, 0, -15.0f};
};
