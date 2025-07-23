#pragma once
#include "KamataEngine.h"
#include "MyMath.h"

using namespace KamataEngine;

class MapChipField;
class Enemy;

enum class LRDirection {
	kRight,
	kLeft,
};

struct CollisionMapInfo {
	bool isCollision = false;
	bool isOnGround = false;
	bool isHitWall = false;
	bool isCeiling = false;
	Vector3 move = {0.0f, 0.0f, 0.0f};
};

enum Corner { kRightBottom, kLeftBottom, kRightTop, kLeftTop, kNumCorner };

class Player {
public:
	void Initialize(Model* model, Camera* camera, const Vector3& position);
	void Update();
	void Draw();
	void InputMove();
	void AnimateTurn();
	void CollisionMapCheck(CollisionMapInfo& Info);
	//void OnCollision(const Enemy* enemy);
	void CheckMapCollision(CollisionMapInfo& Info);
	void CheckMapCollisionUp(CollisionMapInfo& Info);
	void CheckMapCollisionDown(CollisionMapInfo& Info);
	void CheckMapCollisionLeft(CollisionMapInfo& Info);
	void CheckMapCollisionRight(CollisionMapInfo& Info);
	// 死亡管理
    void Die();               // 死亡処理
    bool IsDead() const;      // 死亡状態確認

	const WorldTransform& GetWorldTransform() const { return worldTransform_; }
	const Vector3& GetVelocity() const { return velocity_; }
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	AABB GetAABB();

	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
	static inline const float kBlank = 0.1f;
	static inline const float kTimeTurn = 0.3f;
	static inline const float kGravityAcceleration = 0.01f;
	static inline const float kLimitFallSpeed = 0.5f;
	static inline const float kJumpAcceleration = 0.3f;

private:
	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	bool OnGround_ = true;
	Camera* camera_ = nullptr;
	uint32_t textureHandle_ = 0u;
	Vector3 velocity_ = {};
	float kAcceleration = 0.1f;
	LRDirection lrDirection_ = LRDirection::kRight;
	float turnFirstRotationY_ = 0.0f;
	float turnTimer_ = 0.0f;
	MapChipField* mapChipField_ = nullptr;
	Vector3 GetWorldPosition();
	bool isDead_ = false;
};

Vector3 CornerPosition(const Vector3& center, Corner corner);