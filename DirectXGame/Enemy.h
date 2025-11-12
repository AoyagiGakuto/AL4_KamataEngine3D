#pragma once
#include "KamataEngine.h"
#include "MapChipField.h"
#include "MyMath.h"
#include "Player.h"
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

	void SetTarget(Player* p) { target_ = p; }
	void SetHoming(bool enable) { homing_ = enable; }
	void SetHomingParams(float maxSpeed, float accel, float stopDist) {
		homingMaxSpeed_ = maxSpeed;
		homingAccel_ = accel;
		homingStopDist_ = stopDist;
	}

	// ダメージを受ける
	void TakeDamage(int damage);

	// 死亡しているか
	bool IsDead() const;

	// 動きをスローにする
	void SlowDown(float duration);

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
	Player* target_ = nullptr;

	int hp_ = 5; // HP (今は5発で死ぬように)
	bool homing_ = false;
	float homingMaxSpeed_ = 0.06f; // 最大速度（横移動）
	float homingAccel_ = 0.004f;   // 1フレーム加速量
	float homingStopDist_ = 0.05f; // これ以下の距離で減速・停止
	float slowTimer_ = 0.0f;       // スロー効果の残り時間
};