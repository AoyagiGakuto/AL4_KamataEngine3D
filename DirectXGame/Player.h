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
	void TakeDamage(int damage);
	void CollisionMapCheck(CollisionMapInfo& Info);
	void CheckMapCollision(CollisionMapInfo& Info);
	void CheckMapCollisionUp(CollisionMapInfo& Info);
	void CheckMapCollisionDown(CollisionMapInfo& Info);
	void CheckMapCollisionLeft(CollisionMapInfo& Info);
	void CheckMapCollisionRight(CollisionMapInfo& Info);

	// 位置を瞬間移動させる
	void WarpTo(const Vector3& position);

	// 死亡管理
    void Die();               // 死亡処理
    bool IsDead() const;      // 死亡状態確認
	// チャージ攻撃の準備ができているか
	bool IsChargeAttackReady() const { return isChargeAttackReady_; }
	// チャージ攻撃を消費（GameSceneが呼び出す）
	void ConsumeChargeAttack() { isChargeAttackReady_ = false; }
	// ロックオン処理
	void LockOn(Enemy* target);
	void LockOff();
	bool IsLockedOn() const { return isLockedOn_; }
	Enemy* GetTargetEnemy() const { return targetEnemy_; }

	// ノックアップ
	void Knockback(const Vector3& dir);

	const WorldTransform& GetWorldTransform() const { return worldTransform_; }
	const Vector3& GetVelocity() const { return velocity_; }
	float GetTurnTimer() const { return turnTimer_; }
	float GetHp() const { return (float)hp_; }
	float GetMaxHp() const { return (float)maxHp_; }
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }
	Vector3 GetPosition() const { return worldTransform_.translation_; }
	AABB GetAABB();

	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
	static inline const float kBlank = 0.01f;
	static inline const float kTimeTurn = 0.3f;
	static inline const float kGravityAcceleration = 0.01f;
	static inline const float kLimitFallSpeed = 0.5f;
	static inline const float kJumpAcceleration = 0.3f;
	bool isDead_ = false;

private:
	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	bool OnGround_ = true;
	int jumpCount_ = 0;
	bool isGliding_ = false;
	Camera* camera_ = nullptr;
	uint32_t textureHandle_ = 0u;
	Vector3 velocity_ = {};
	float kAcceleration = 0.1f;
	LRDirection lrDirection_ = LRDirection::kRight;
	float turnFirstRotationY_ = 0.0f;
	float turnTimer_ = 0.0f;
	MapChipField* mapChipField_ = nullptr;
	Vector3 GetWorldPosition();
	bool input = false;
	Enemy* targetEnemy_ = nullptr;
	bool isLockedOn_ = false;
	// K長押しチャージ用
	float chargeTimer_ = 0.0f;
	bool isChargeAttackReady_ = false;
	static inline const float kChargeAttackTime = 3.0f; // 3秒
	int hp_ = 100;
	int maxHp_ = 100;
	float knockbackTimer_ = 0.0f;
};

Vector3 CornerPosition(const Vector3& center, Corner corner);