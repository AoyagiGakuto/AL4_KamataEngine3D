#pragma once
#include "KamataEngine.h"
#include "MapChipField.h"
#include "MyMath.h"
#include <numbers>

using namespace KamataEngine;

class MapChipField;
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
	static inline const float kWalkSpeed = 0.01f;                                     // 敵の移動速度
	static inline const float kWalkMotionAngelStart = 0.0f;                           // 通常姿勢
	static inline const float kWalkMotionAngelEnd = std::numbers::pi_v<float> / 6.0f; // 30度
	static inline const float kWalkMotionTime = 2.0f;                                 // 敵の歩行モーションの時間
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
	static inline const float kBlank = 0.01f;      // ぶるぶる防止
	static inline const float kFallLimit = 0.2f;   // 最大落下速度
	static inline const float kGravityAcc = 0.01f; // 重力

private:

	struct CollisionInfo {
		Vector3 move{0, 0, 0};
		bool isOnGround = false;
		bool isCeiling = false;
		bool isHitWall = false;
	};

	void CollisionMapCheck(CollisionInfo& info);
	void CheckMapCollisionUp(CollisionInfo& info);
	void CheckMapCollisionDown(CollisionInfo& info);
	void CheckMapCollisionLeft(CollisionInfo& info);
	void CheckMapCollisionRight(CollisionInfo& info);

	float walkTimer_ = 0.0f;                                                          // 敵の歩行モーションのタイマー
	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	Camera* camera_ = nullptr;
	Vector3 velocity_ = {};
	MapChipField* mapChipField_ = nullptr;
};