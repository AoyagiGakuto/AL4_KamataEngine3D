#pragma once
#include "KamataEngine.h"
#include "MyMath.h"

using namespace KamataEngine;

class Bullet {
public:
	void Initialize(Model* model, Camera* camera, const Vector3& pos, const Vector3& dir);
	void Update();
	void Draw();
	bool IsAlive() const { return alive_; }
	AABB GetAABB() const;

	// 当たり判定用
	void Kill() { alive_ = false; }

private:
	WorldTransform transform_;
	Vector3 direction_ = {1.0f, 0.0f, 0.0f};
	float speed_ = 0.25f;
	float lifetime_ = 2.0f; // 秒
	bool alive_ = true;

	Model* model_ = nullptr;
	Camera* camera_ = nullptr;
};
