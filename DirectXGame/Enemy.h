#pragma once
#include "KamataEngine.h"
#include "MapChipField.h"
#include "MyMath.h"
#include "Player.h"
#include <numbers>
#include <list>

using namespace KamataEngine;

class MapChipField;
class Player;

class Enemy {
public:
	enum class Type {
		kNormal,       // 普通の敵
		kHoming,       // 追いかけてくる敵
		kFlyingSupport // 飛行＆回復する敵
	};

	void Initialize(Model* model, Model* modelHpBar, Model* modelHp, Camera* camera, const Vector3& position, Type type = Type::kNormal);
	void Update();
	void Draw();

	void HealNearbyEnemies(std::list<Enemy*>& enemies);

	AABB GetAABB();

	Type GetType() const { return type_; }

	// ワールド変換取得
	const WorldTransform& GetWorldTransform() const { return worldTransform_; }

	// マップチップフィールドのセット
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }
	void OnCollision(const Player* player); 
	void SetScale(const Vector3& scale) { worldTransform_.scale_ = scale; }
	void SetRotationY(float y) { worldTransform_.rotation_.y = y; }
	void SetTarget(Player* p) { target_ = p; }
	void TakeDamage(int damage);
	void SlowDown(float duration);
	bool IsDead() const;
	bool IsReadyToFire();

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
	float shotTimer_ = 0.0f;

	WorldTransform worldTransform_;
	WorldTransform worldTransformHpBar_; // 枠用
	WorldTransform worldTransformHp_;
	Model* model_ = nullptr;
	Model* modelHpBar_ = nullptr;
	Model* modelHp_ = nullptr;
	Camera* camera_ = nullptr;
	Vector3 velocity_ = {};
	MapChipField* mapChipField_ = nullptr;
	Player* target_ = nullptr;

	float maxHp_ = 5.0f;
	int hp_ = 5; // HP (今は5発で死ぬように)
	float slowTimer_ = 0.0f;       // スロー効果の残り時間

	Type type_ = Type::kNormal; // 自分のタイプ

	float homingMaxSpeed_ = 0.06f; // 最大速度（横移動）
	float homingAccel_ = 0.004f;   // 1フレーム加速量
	float homingStopDist_ = 0.05f; // これ以下の距離で減速・停止
	
	float baseHeight_ = 0.0f;   // 飛行時の基準の高さ
	float healTimer_ = 0.0f;    // 回復スキルのクールダウン
	Enemy* healTarget_ = nullptr; // 回復しに行く対象

	// 定数
	static inline const float kFireInterval = 2.0f; // 2秒に1回撃つ
	static inline const float kHealRange = 5.0f;    // 回復が届く範囲
	static inline const float kHealCooldown = 3.0f; // 回復の間隔（秒）
};