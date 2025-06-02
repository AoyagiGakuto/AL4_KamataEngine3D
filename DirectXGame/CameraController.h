#pragma once
#include "KamataEngine.h"
#include "Player.h"

using namespace KamataEngine;
using namespace MathUtility;

/*
struct Rect {
	float lef= 0.0f;
	float top =;
	float right;
	float bottom;
}; */

class CameraController {
public:
	void Initialize();
	void Update();
	void SetTarget(Player* target) { target_ = target; };
	void Reset();

	const Camera& GetViewProjection() const { return camera_; }
	Matrix4x4 matView_;       // ビュー行列
	Matrix4x4 matProjection_; // プロジェクション行列

private:
	// カメラ
	Camera camera_;
	Player* target_ = nullptr;
	Vector3 targetOffset_ = {0, 0, -15.0f};
};
