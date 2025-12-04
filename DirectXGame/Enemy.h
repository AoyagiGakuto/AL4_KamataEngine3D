#pragma once
#include "KamataEngine.h"
#include "MapChipField.h"
#include "MyMath.h"
#include "Player.h"
#include <list>
#include <numbers>

class MapChipField;
class Player;

class Enemy {
public:
	enum class Type {
		kNormal,       // 普通の敵
		kHoming,       // 追いかけてくる敵
		kFlyingSupport // 飛行＆回復する敵
	};

	// 初期化
	void Initialize(
	    KamataEngine::Model* model, KamataEngine::Model* modelHpBar, KamataEngine::Model* modelHp, KamataEngine::Camera* camera, const KamataEngine::Vector3& position, Type type = Type::kNormal);

	// 更新
	void Update();

	// 描画
	void Draw();

	/*
	// --- なんちゃってAI ---
	*/

	// 周囲の敵を回復させる
	void HealNearbyEnemies(std::list<Enemy*>& enemies);
	// ダメージを受ける
	void TakeDamage(int damage);
	// スロー効果付与
	void SlowDown(float duration);
	// ノックバック
	void Knockback(const KamataEngine::Vector3& dir);
	// ヒットストップ（一時停止）
	void ApplyHitStop(float duration);

	/*
	// --- ゲッターとセッター ---
	*/

	AABB GetAABB();
	Type GetType() const { return type_; }
	const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; }

	// マップチップフィールドのセット
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }
	void OnCollision(const Player* player);

	void SetScale(const KamataEngine::Vector3& scale) { worldTransform_.scale_ = scale; }
	void SetRotationY(float y) { worldTransform_.rotation_.y = y; }
	void SetTarget(Player* p) { target_ = p; }

	bool IsDead() const;
	bool IsReadyToFire();

	// 定数パラメータ

	// 敵の移動速度
	static inline const float kWalkSpeed = 0.01f;
	// 通常姿勢
	static inline const float kWalkMotionAngelStart = 0.0f;
	// 30度
	static inline const float kWalkMotionAngelEnd = std::numbers::pi_v<float> / 6.0f;
	// 敵の歩行モーションの時間
	static inline const float kWalkMotionTime = 2.0f;
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
	// ぶるぶる防止
	static inline const float kBlank = 0.01f;
	// 最大落下速度
	static inline const float kFallLimit = 0.2f;
	// 重力
	static inline const float kGravityAcc = 0.01f;

private:
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
	// --- 状態データ ---
	*/

	// 敵の歩行モーションのタイマー
	float walkTimer_ = 0.0f;
	float shotTimer_ = 0.0f;
	float hitStopTimer_ = 0.0f;

	float maxHp_ = 5.0f;
	int hp_ = 5;
	// スロー効果の残り時間
	float slowTimer_ = 0.0f;
	float knockbackTimer_ = 0.0f;

	// 自分のタイプ
	Type type_ = Type::kNormal;

	// AI用パラメータ（追尾と回復）
	float homingMaxSpeed_ = 0.06f; // 最大速度（横移動）
	float homingAccel_ = 0.004f;   // 1フレーム加速量
	float homingStopDist_ = 0.05f; // これ以下の距離で減速・停止
	float baseHeight_ = 0.0f;      // 飛行時の基準の高さ
	float healTimer_ = 0.0f;       // 回復スキルのクールダウン
	Enemy* healTarget_ = nullptr;  // 回復しに行く対象

	// 外部参照
	MapChipField* mapChipField_ = nullptr;
	Player* target_ = nullptr;

	/*
	// --- 描画・変換 ---
	*/

	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::WorldTransform worldTransformHpBar_; // 枠用
	KamataEngine::WorldTransform worldTransformHp_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* modelHpBar_ = nullptr;
	KamataEngine::Model* modelHp_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;
	KamataEngine::Vector3 velocity_ = {};

	// 定数
	static inline const float kFireInterval = 2.0f; // 2秒に1回撃つ
	static inline const float kHealRange = 5.0f;    // 回復が届く範囲
	static inline const float kHealCooldown = 3.0f; // 回復の間隔
};