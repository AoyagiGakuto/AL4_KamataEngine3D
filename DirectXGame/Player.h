#pragma once
#include "KamataEngine.h"
#include "MyMath.h"

class MapChipField;
class Enemy;

// 左右の向き
enum class LRDirection {
	kRight,
	kLeft,
};

// 衝突判定の結果格納用
struct CollisionMapInfo {
	bool isCollision = false;
	bool isOnGround = false;
	bool isHitWall = false;
	bool isCeiling = false;
	KamataEngine::Vector3 move = {0.0f, 0.0f, 0.0f};
};

// 角の判定用
enum Corner { kRightBottom, kLeftBottom, kRightTop, kLeftTop, kNumCorner };

class Player {
public:
	// 初期化
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	
	// 更新
	void Update();
	
	// 描画
	void Draw();


	/*
	// --- アクション・制御 ---
	*/

	// 移動入力処理
	void InputMove();
	// 振り向きアニメーション
	void AnimateTurn();
	// ダメージ処理
	void TakeDamage(int damage);
	// 死亡処理
	void Die();
	// ノックバック
	void Knockback(const KamataEngine::Vector3& dir);

	/*
	// --- 衝突判定 ---
	*/

	// マップチップとの衝突チェック（全体）
	void CollisionMapCheck(CollisionMapInfo& Info);

	// 4方向それぞれの衝突チェック
	void CheckMapCollision(CollisionMapInfo& Info);
	void CheckMapCollisionUp(CollisionMapInfo& Info);
	void CheckMapCollisionDown(CollisionMapInfo& Info);
	void CheckMapCollisionLeft(CollisionMapInfo& Info);
	void CheckMapCollisionRight(CollisionMapInfo& Info);

	// 位置を瞬間移動させる
	void WarpTo(const KamataEngine::Vector3& position);

	// 死亡管理
    void Die();
    bool IsDead() const;

	// チャージ攻撃の準備ができているか
	bool IsChargeAttackReady() const { return isChargeAttackReady_; }

	// チャージ攻撃を消費
	void ConsumeChargeAttack() { isChargeAttackReady_ = false; }

	// ロックオン処理
	void LockOn(Enemy* target);
	void LockOff();
	bool IsLockedOn() const { return isLockedOn_; }
	Enemy* GetTargetEnemy() const { return targetEnemy_; }

	// ノックアップ
	void Knockback(const KamataEngine::Vector3& dir);

	const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; }
	const KamataEngine::Vector3& GetVelocity() const { return velocity_; }
	float GetTurnTimer() const { return turnTimer_; }
	float GetHp() const { return (float)hp_; }
	float GetMaxHp() const { return (float)maxHp_; }
	KamataEngine::Vector3 GetPosition() const { return worldTransform_.translation_; }
	AABB GetAABB();

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
	static inline const float kBlank = 0.01f;
	static inline const float kTimeTurn = 0.3f;
	static inline const float kGravityAcceleration = 0.01f;
	static inline const float kLimitFallSpeed = 0.5f;
	static inline const float kJumpAcceleration = 0.3f;
	
	bool isDead_ = false;

private:

	// ワールド座標取得（中心補正あり）
	KamataEngine::Vector3 GetWorldPosition();

	/*
	// --- モデル ---
	*/

	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;

	/*
	// --- 移動 ---
	*/

	KamataEngine::Vector3 velocity_ = {};
	float kAcceleration = 0.1f;
	bool OnGround_ = true;
	int jumpCount_ = 0;
	bool isGliding_ = false;
	bool input = false;

	// 向き制御
	LRDirection lrDirection_ = LRDirection::kRight;
	float turnFirstRotationY_ = 0.0f;
	float turnTimer_ = 0.0f;

	MapChipField* mapChipField_ = nullptr;

	/*
	// --- バトル・ステータス ---
	*/

	// HP
	int hp_ = 100;
	int maxHp_ = 100;
	float knockbackTimer_ = 0.0f;

	// ロックオン
	Enemy* targetEnemy_ = nullptr;
	bool isLockedOn_ = false;

	// チャージ攻撃
	float chargeTimer_ = 0.0f;
	bool isChargeAttackReady_ = false;
	static inline const float kChargeAttackTime = 3.0f; // 3秒
};

KamataEngine::Vector3 CornerPosition(const KamataEngine::Vector3& center, Corner corner);