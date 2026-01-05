#pragma once
#include "KamataEngine.h"
#include "MapChipField.h"
#include "MyMath.h"
#include "Player.h"
#include <list>
#include <numbers>

class MapChipField;
class Player;

// ==========================================
// 基底クラス (親)
// ==========================================
class Enemy {
public:

	virtual ~Enemy() = default;

	// 初期化
	virtual void Initialize(KamataEngine::Model* model, KamataEngine::Model* modelHpBar, KamataEngine::Model* modelHp, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	// 更新
	void Update();

	// 描画
	void Draw();

	// 固有の行動 (飛行敵の回復など)
	virtual void PerformUniqueAction(std::list<Enemy*>& enemies) { (void)enemies; }

	// ダメージを受ける
	void TakeDamage(int damage);

	// スロー効果付与
	void SlowDown(float duration);
	
	// ノックバック
	void Knockback(const KamataEngine::Vector3& dir);
	
	// ヒットストップ
	void ApplyHitStop(float duration);
	
	// 打ち上げ
	void Launch(float power);
	
	// 空中で攻撃を受けた時の浮遊処理
	void OnAirHit(float time);
	
	// 叩きつけ
	void SlamDown();
	
	// 重力を元に戻す
	void ResetGravity();

	AABB GetAABB();
	const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; }
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }
	void SetScale(const KamataEngine::Vector3& scale) { worldTransform_.scale_ = scale; }
	void SetRotationY(float y) { worldTransform_.rotation_.y = y; }
	void SetTarget(Player* p) { target_ = p; }
	float GetHp() const { return (float)hp_; }
	float GetMaxHp() const { return maxHp_; }
	bool IsDead() const;
	bool IsReadyToFire();

	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
	static inline const float kBlank = 0.01f;
	static inline const float kGravityAcc = 0.01f;
	static inline const float kFireInterval = 2.0f;

protected:
	// 移動
	virtual void Move() = 0;

	// 衝突判定系
	struct CollisionInfo {
		KamataEngine::Vector3 move{0, 0, 0};
		bool isOnGround = false;
		bool isCeiling = false;
		bool isHitWall = false;
	};

	void CollisionMapCheck(CollisionInfo& info);
	void CheckMapCollisionUp(CollisionInfo& info);
	void CheckMapCollisionDown(CollisionInfo& info);
	void CheckMapCollisionLeft(CollisionInfo& info);
	void CheckMapCollisionRight(CollisionInfo& info);
	
	/*
	// --- メンバ変数 ---
	*/

	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::WorldTransform worldTransformHpBar_;
	KamataEngine::WorldTransform worldTransformHp_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* modelHpBar_ = nullptr;
	KamataEngine::Model* modelHp_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;
	KamataEngine::Vector3 velocity_ = {};

	MapChipField* mapChipField_ = nullptr;
	Player* target_ = nullptr;

	float maxHp_ = 5.0f;
	int hp_ = 5;
	float walkTimer_ = 0.0f;
	float shotTimer_ = 0.0f;
	float hitStopTimer_ = 0.0f;
	float slowTimer_ = 0.0f;
	float knockbackTimer_ = 0.0f;
	float speedMultiplier_ = 1.0f; // Updateの内の速度倍率

	float gravityScale_ = 1.0f; // 重力倍率
	float suspendTimer_ = 0.0f;

	// 弾のフラグ
	bool canShoot_ = false;
};

// ==========================================
// 通常の敵 (Normal)
// ==========================================
class NormalEnemy : public Enemy {
public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Model* modelHpBar, KamataEngine::Model* modelHp, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) override;

protected:
	void Move() override;

private:
	static inline const float kWalkSpeed = 0.01f;
	static inline const float kWalkMotionTime = 2.0f;
	static inline const float kWalkMotionAngelStart = 0.0f;
	static inline const float kWalkMotionAngelEnd = std::numbers::pi_v<float> / 6.0f;
};

// ==========================================
// 追尾する敵
// ==========================================
class HomingEnemy : public Enemy {
public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Model* modelHpBar, KamataEngine::Model* modelHp, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) override;

protected:
	void Move() override;

private:
	float homingMaxSpeed_ = 0.06f;
	float homingAccel_ = 0.004f;
	float homingStopDist_ = 0.05f;
};

// ==========================================
// 飛行・支援する敵
// ==========================================
class FlyingEnemy : public Enemy {
public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Model* modelHpBar, KamataEngine::Model* modelHp, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) override;

	// 回復行動
	void PerformUniqueAction(std::list<Enemy*>& enemies) override;

protected:
	void Move() override;

private:
	float baseHeight_ = 0.0f;
	float healTimer_ = 0.0f;
	Enemy* healTarget_ = nullptr;

	static inline const float kHealRange = 5.0f;
	static inline const float kHealCooldown = 3.0f;
};