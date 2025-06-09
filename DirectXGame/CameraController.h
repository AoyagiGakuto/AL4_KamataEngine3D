#pragma once
#include "KamataEngine.h"
#include "Player.h"

using namespace KamataEngine;
using namespace MathUtility;

class CameraController {
public:

	struct Rect {
		float left = 0.0f;
		float right = 1.0;
		float bottom = 0.0f;
		float top = 1.0f;
	};

	void Initialize();
	void Update();
	void SetTarget(Player* target) { target_ = target; };
	void SetMovableArea(const Rect& area) { movebleArea_ = area; }
	void Reset();

	const Camera& GetViewProjection() const { return camera_; }
	Matrix4x4 matView_;       // ビュー行列
	Matrix4x4 matProjection_; // プロジェクション行列

private:
	// カメラ
	Camera camera_;
	Player* target_ = nullptr;
	Vector3 targetOffset_ = {0, 0, -15.0f};
	// カメラの目標座標
	KamataEngine::Vector3 targetPosition_ = {0, 0, 0};
	Rect movebleArea_ = {0, 100, 0, 100};
	// 座標線形保管割合
	static inline const float kInterpolationRate = 0.1f;
};
