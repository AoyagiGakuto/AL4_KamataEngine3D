#pragma once
#include "KamataEngine.h"
#include "MapChipField.h"
#include "MyMath.h"
#include <numbers>

using namespace KamataEngine;

class Player;

class Enemy {
public:
	void Initialize(Model* model, Camera* camera, const Vector3& position);
	void Update();
	void Draw();

	AABB GetAABB();

	// ワールド変換取得
	const WorldTransform& GetWorldTransform() const { return worldTransform_; }

	// マップチップフィールドのセット
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	void OnCollision(const Player* player); 
	void SetScale(const Vector3& scale) { worldTransform_.scale_ = scale; }
	void SetRotationY(float y) { worldTransform_.rotation_.y = y; }

private:
	static inline const float kWalkSpeed = 0.01f;                                     // 敵の移動速度
	static inline const float kWalkMotionAngelStart = 0.0f;                           // 通常姿勢
	static inline const float kWalkMotionAngelEnd = std::numbers::pi_v<float> / 6.0f; // 30度
	static inline const float kWalkMotionTime = 2.0f;                                 // 敵の歩行モーションの時間
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
	float walkTimer_ = 0.0f;                                                          // 敵の歩行モーションのタイマー
	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	Camera* camera_ = nullptr;
	Vector3 velocity_ = {};
	MapChipField* mapChipField_ = nullptr;
};