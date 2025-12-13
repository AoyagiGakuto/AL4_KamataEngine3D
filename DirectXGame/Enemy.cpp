#define NOMINMAX
#include "Enemy.h"
#include <algorithm>
#include <cassert>

using namespace KamataEngine::MathUtility;
using namespace KamataEngine;

// --------------------------------------------------------------------------
// 初期化処理
// --------------------------------------------------------------------------

void Enemy::Initialize(Model* model, Model* modelHpBar, Model* modelHp, Camera* camera, const Vector3& position) {
	assert(model);
	model_ = model;
	modelHpBar_ = modelHpBar;
	modelHp_ = modelHp;
	camera_ = camera;

	worldTransform_.Initialize();
	worldTransformHpBar_.Initialize();
	worldTransformHp_.Initialize();

	// HPバーのスケール設定
	Vector3 hpBarScale = {0.15f, 0.15f, 0.15f};
	worldTransformHpBar_.scale_ = hpBarScale;
	worldTransformHp_.scale_ = hpBarScale;

	worldTransform_.translation_ = position;

	walkTimer_ = 0.0f;
	slowTimer_ = 0.0f;
	canShoot_ = false; // デフォルトは撃たない
}

// --------------------------------------------------------------------------
// 更新処理
// --------------------------------------------------------------------------

void Enemy::Update() {
	
	/*
	// --- ヒットストップ処理 ---
	*/

	if (hitStopTimer_ > 0.0f) {
		hitStopTimer_ -= 1.0f / 60.0f;
		return;
	}

	if (shotTimer_ > 0.0f) {
		shotTimer_ -= 1.0f / 60.0f;
	}

	/*
	// --- スロー状態の処理 ---
	*/

	speedMultiplier_ = 1.0f;
	if (slowTimer_ > 0.0f) {
		slowTimer_ -= 1.0f / 60.0f;
		speedMultiplier_ = 0.3f;
	}

	walkTimer_ += 1.0f / 60.0f;

	/* 
	// --- ノックバック or 通常移動 ---
	*/

	if (knockbackTimer_ > 0.0f) {
		knockbackTimer_ -= 1.0f / 60.0f;

		// ノックバック中は操作不能で重力のみかかる
		velocity_.y -= kGravityAcc;

		CollisionInfo info;
		info.move = velocity_;
		CollisionMapCheck(info);
		worldTransform_.translation_ += info.move;
	} else {
		Move();
	}

	/* 
	// --- HPバーの座標更新 ---
	*/

	Vector3 barPos = worldTransform_.translation_;
	barPos.y += 1.5f;

	worldTransformHpBar_.translation_ = barPos;
	worldTransformHp_.translation_ = barPos;

	float hpRatio = (float)hp_ / maxHp_;
	hpRatio = std::clamp(hpRatio, 0.0f, 1.0f);

	float baseScaleX = 0.15f;
	worldTransformHp_.scale_.x = baseScaleX * hpRatio;
	worldTransformHp_.scale_.y = 0.15f;
	worldTransformHp_.scale_.z = 0.15f;

	float modelHalfWidth = 3.0f;
	float shiftAmount = (1.0f - hpRatio) * modelHalfWidth * baseScaleX;
	worldTransformHp_.translation_.x -= shiftAmount;

	// 行列更新 (HPバー枠)
	worldTransformHpBar_.matWorld_ = MakeAffineMatrix(worldTransformHpBar_.scale_, worldTransformHpBar_.rotation_, worldTransformHpBar_.translation_);
	worldTransformHpBar_.TransferMatrix();

	// 行列更新 (HPバー中身)
	worldTransformHp_.matWorld_ = MakeAffineMatrix(worldTransformHp_.scale_, worldTransformHp_.rotation_, worldTransformHp_.translation_);
	worldTransformHp_.TransferMatrix();

	// 行列更新 (本体)
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

// --------------------------------------------------------------------------
// 描画処理
// --------------------------------------------------------------------------

void Enemy::Draw() {
	if (model_ && camera_) {
		model_->Draw(worldTransform_, *camera_);
		if (hp_ > 0) {
			if (modelHpBar_) {
				modelHpBar_->Draw(worldTransformHpBar_, *camera_);
			}

			if (modelHp_) {
				modelHp_->Draw(worldTransformHp_, *camera_);
			}
		}
	}
}

// --------------------------------------------------------------------------
// 共通アクション・パラメータ
// --------------------------------------------------------------------------

void Enemy::TakeDamage(int damage) {
	hp_ -= damage;
	if (hp_ > (int)maxHp_) {
		hp_ = (int)maxHp_;
	}
}

bool Enemy::IsDead() const { return hp_ <= 0; }

void Enemy::SlowDown(float duration) {
	if (slowTimer_ < duration) {
		slowTimer_ = duration;
	}
}

void Enemy::ApplyHitStop(float duration) { hitStopTimer_ = duration; }

void Enemy::Knockback(const Vector3& dir) {
	if (knockbackTimer_ > 0.0f) {
		return;
	}

	velocity_.x = dir.x * 0.1f;
	velocity_.y = 0.15f;
	knockbackTimer_ = 0.5f;
}

bool Enemy::IsReadyToFire() {
	if (!canShoot_){
		return false;
	}

	if (shotTimer_ <= 0.0f) {
		shotTimer_ = kFireInterval;
		return true;
	}
	return false;
}

AABB Enemy::GetAABB() {
	AABB aabb;
	aabb.min = {worldTransform_.translation_.x - kWidth / 2.0f, worldTransform_.translation_.y - kHeight / 2.0f, worldTransform_.translation_.z - kWidth / 2.0f};
	aabb.max = {worldTransform_.translation_.x + kWidth / 2.0f, worldTransform_.translation_.y + kHeight / 2.0f, worldTransform_.translation_.z + kWidth / 2.0f};
	return aabb;
}

// --------------------------------------------------------------------------
// マップ衝突判定
// --------------------------------------------------------------------------

void Enemy::CollisionMapCheck(CollisionInfo& info) {
	CheckMapCollisionDown(info);
	CheckMapCollisionUp(info);
	CheckMapCollisionLeft(info);
	CheckMapCollisionRight(info);
}

void Enemy::CheckMapCollisionDown(CollisionInfo& info) {
	if (info.move.y >= 0.0f) {
		return;
	}

	assert(mapChipField_);

	float foot = worldTransform_.translation_.y - kHeight / 2.0f;
	float newFoot = foot + info.move.y;
	float playerLeft = worldTransform_.translation_.x - kWidth / 2.0f;
	float playerRight = worldTransform_.translation_.x + kWidth / 2.0f;

	MapChipField::IndexSet indexSet_L = mapChipField_->GetMapChipIndexSetByPosition({playerLeft, newFoot, 0.0f});
	MapChipField::IndexSet indexSet_R = mapChipField_->GetMapChipIndexSetByPosition({playerRight - kBlank, newFoot, 0.0f});

	MapChipType type_L = mapChipField_->GetMapChipTypeByIndex(indexSet_L.xIndex, indexSet_L.yIndex);
	MapChipType type_R = mapChipField_->GetMapChipTypeByIndex(indexSet_R.xIndex, indexSet_R.yIndex);

	if (type_L == MapChipType::kBlock || type_R == MapChipType::kBlock) {
		float blockTop = -FLT_MAX;
		if (type_L == MapChipType::kBlock) {
			blockTop = std::max(blockTop, mapChipField_->GetRectByIndex(indexSet_L.xIndex, indexSet_L.yIndex).top);
		}

		if (type_R == MapChipType::kBlock) {
			blockTop = std::max(blockTop, mapChipField_->GetRectByIndex(indexSet_R.xIndex, indexSet_R.yIndex).top);
		}

		worldTransform_.translation_.y = blockTop + kHeight / 2.0f;
		info.move.y = 0.0f;
		velocity_.y = 0.0f;
		info.isOnGround = true;
	}
}

void Enemy::CheckMapCollisionUp(CollisionInfo& info) {
	if (info.move.y <= 0.0f) {
		return;
	}

	assert(mapChipField_);

	float head = worldTransform_.translation_.y + kHeight / 2.0f;
	float newHead = head + info.move.y;
	float playerLeft = worldTransform_.translation_.x - kWidth / 2.0f;
	float playerRight = worldTransform_.translation_.x + kWidth / 2.0f;

	MapChipField::IndexSet indexSet_L = mapChipField_->GetMapChipIndexSetByPosition({playerLeft, newHead, 0.0f});
	MapChipField::IndexSet indexSet_R = mapChipField_->GetMapChipIndexSetByPosition({playerRight - kBlank, newHead, 0.0f});

	MapChipType type_L = mapChipField_->GetMapChipTypeByIndex(indexSet_L.xIndex, indexSet_L.yIndex);
	MapChipType type_R = mapChipField_->GetMapChipTypeByIndex(indexSet_R.xIndex, indexSet_R.yIndex);

	if (type_L == MapChipType::kBlock || type_R == MapChipType::kBlock) {
		float blockBottom = FLT_MAX;
		if (type_L == MapChipType::kBlock) {
			blockBottom = std::min(blockBottom, mapChipField_->GetRectByIndex(indexSet_L.xIndex, indexSet_L.yIndex).bottom);
		}
		
		if (type_R == MapChipType::kBlock) {
			blockBottom = std::min(blockBottom, mapChipField_->GetRectByIndex(indexSet_R.xIndex, indexSet_R.yIndex).bottom);
		}

		worldTransform_.translation_.y = blockBottom - kHeight / 2.0f;
		info.move.y = 0.0f;
		velocity_.y = 0.0f;
		info.isCeiling = true;
	}
}

void Enemy::CheckMapCollisionLeft(CollisionInfo& info) {
	if (info.move.x >= 0.0f) {
		return;
	}

	assert(mapChipField_);

	float left = worldTransform_.translation_.x - kWidth / 2.0f;
	float newLeft = left + info.move.x;
	float playerTop = worldTransform_.translation_.y + kHeight / 2.0f;
	float playerBottom = worldTransform_.translation_.y - kHeight / 2.0f;

	MapChipField::IndexSet indexSet_T = mapChipField_->GetMapChipIndexSetByPosition({newLeft, playerTop - kBlank, 0.0f});
	MapChipField::IndexSet indexSet_B = mapChipField_->GetMapChipIndexSetByPosition({newLeft, playerBottom, 0.0f});

	MapChipType type_T = mapChipField_->GetMapChipTypeByIndex(indexSet_T.xIndex, indexSet_T.yIndex);
	MapChipType type_B = mapChipField_->GetMapChipTypeByIndex(indexSet_B.xIndex, indexSet_B.yIndex);

	if (type_T == MapChipType::kBlock || type_B == MapChipType::kBlock) {
		float blockRight = -FLT_MAX;
		if (type_T == MapChipType::kBlock) {
			blockRight = std::max(blockRight, mapChipField_->GetRectByIndex(indexSet_T.xIndex, indexSet_T.yIndex).right);
		}
		
		if (type_B == MapChipType::kBlock) {
			blockRight = std::max(blockRight, mapChipField_->GetRectByIndex(indexSet_B.xIndex, indexSet_B.yIndex).right);
		}

		worldTransform_.translation_.x = blockRight + kWidth / 2.0f;
		info.move.x = 0.0f;
		info.isHitWall = true;
	}
}

void Enemy::CheckMapCollisionRight(CollisionInfo& info) {
	if (info.move.x <= 0.0f) {
		return;
	}

	assert(mapChipField_);

	float right = worldTransform_.translation_.x + kWidth / 2.0f;
	float newRight = right + info.move.x;
	float playerTop = worldTransform_.translation_.y + kHeight / 2.0f;
	float playerBottom = worldTransform_.translation_.y - kHeight / 2.0f;

	MapChipField::IndexSet indexSet_T = mapChipField_->GetMapChipIndexSetByPosition({newRight, playerTop - kBlank, 0.0f});
	MapChipField::IndexSet indexSet_B = mapChipField_->GetMapChipIndexSetByPosition({newRight, playerBottom, 0.0f});

	MapChipType type_T = mapChipField_->GetMapChipTypeByIndex(indexSet_T.xIndex, indexSet_T.yIndex);
	MapChipType type_B = mapChipField_->GetMapChipTypeByIndex(indexSet_B.xIndex, indexSet_B.yIndex);

	if (type_T == MapChipType::kBlock || type_B == MapChipType::kBlock) {
		float blockLeft = FLT_MAX;
		if (type_T == MapChipType::kBlock) {
			blockLeft = std::min(blockLeft, mapChipField_->GetRectByIndex(indexSet_T.xIndex, indexSet_T.yIndex).left);
		}

		if (type_B == MapChipType::kBlock) {
			blockLeft = std::min(blockLeft, mapChipField_->GetRectByIndex(indexSet_B.xIndex, indexSet_B.yIndex).left);
		}
		worldTransform_.translation_.x = blockLeft - kWidth / 2.0f;
		info.move.x = 0.0f;
		info.isHitWall = true;
	}
}

// ==========================================================================
// 通常タイプ
// ==========================================================================

void NormalEnemy::Initialize(Model* model, Model* modelHpBar, Model* modelHp, Camera* camera, const Vector3& position) {
	Enemy::Initialize(model, modelHpBar, modelHp, camera, position);
	velocity_ = {-kWalkSpeed, 0.0f, 0.0f};
	hp_ = 5;
	maxHp_ = 5.0f;
}

void NormalEnemy::Move() {
	// 歩行アニメーション (体を左右に振る)
	float param = std::sin(walkTimer_ * (std::numbers::pi_v<float> * 2.0f / kWalkMotionTime)) * (kWalkMotionAngelEnd - kWalkMotionAngelStart) + kWalkMotionAngelStart;
	float degree = kWalkMotionAngelStart + kWalkMotionAngelEnd * (param + 1.0f) / 2.0f;
	worldTransform_.rotation_.x = (degree);

	// 重力
	velocity_.y -= kGravityAcc;

	// 移動量計算
	CollisionInfo info;
	info.move = velocity_ * speedMultiplier_;
	CollisionMapCheck(info);

	// 壁に当たったら反転
	if (info.isHitWall) {
		velocity_.x *= -1.0f;
	}

	// 座標反映
	worldTransform_.translation_ += info.move;

	// 進行方向に向きを変える
	if (velocity_.x > 0.001f) {
		worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	}

	else if (velocity_.x < -0.001f) {
		worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;
	}
}

// ==========================================================================
// 追尾タイプ
// ==========================================================================

void HomingEnemy::Initialize(Model* model, Model* modelHpBar, Model* modelHp, Camera* camera, const Vector3& position) {
	Enemy::Initialize(model, modelHpBar, modelHp, camera, position);
	velocity_ = {0.0f, 0.0f, 0.0f};
	hp_ = 3;
	maxHp_ = 3.0f;
}

void HomingEnemy::Move() {
	worldTransform_.rotation_.x = 0.0f;

	// プレイヤーに向かって加速
	if (target_) {
		float dx = target_->GetWorldTransform().translation_.x - worldTransform_.translation_.x;
		float desiredVx = 0.0f;

		// 一定距離以上離れていれば動く
		if (std::fabs(dx) > homingStopDist_) {
			desiredVx = (dx > 0 ? homingMaxSpeed_ : -homingMaxSpeed_);
		}

		// 慣性をつけて速度調整
		float dv = desiredVx - velocity_.x;
		dv = std::clamp(dv, -homingAccel_, homingAccel_);
		velocity_.x += dv * speedMultiplier_;
	}

	// 重力
	velocity_.y -= kGravityAcc;

	// 移動と衝突判定
	CollisionInfo info;
	info.move = velocity_ * speedMultiplier_;
	CollisionMapCheck(info);
	worldTransform_.translation_ += info.move;

	// 向き
	if (velocity_.x > 0.001f) {
		worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	}

	else if (velocity_.x < -0.001f){
		worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;
    }
}

// ==========================================================================
// 飛行回復支援タイプ
// ==========================================================================

void FlyingEnemy::Initialize(Model* model, Model* modelHpBar, Model* modelHp, Camera* camera, const Vector3& position) {
	Enemy::Initialize(model, modelHpBar, modelHp, camera, position);
	baseHeight_ = position.y;
	hp_ = 3;
	maxHp_ = 3.0f;
	healTimer_ = 0.0f;
	canShoot_ = true; // 今は飛行タイプのみ弾を撃てる設定
}

void FlyingEnemy::Move() {
	// ふわふわ浮く
	float hoverOffset = std::sin(walkTimer_ * 2.0f) * 0.5f;
	Vector3 targetPos = worldTransform_.translation_;
	bool isHealingMode = false;

	// 回復ターゲットがいるかチェック
	if (healTarget_ && !healTarget_->IsDead()) {
		targetPos = healTarget_->GetWorldTransform().translation_;
		isHealingMode = true;
	} else if (target_) {
		// 攻撃対象
		targetPos = target_->GetWorldTransform().translation_;
		isHealingMode = false;
	}

	velocity_.x = 0.0f;
	if (isHealingMode) {
		// 回復モード
		float dx = targetPos.x - worldTransform_.translation_.x;
		if (std::fabs(dx) > 1.5f) {
			velocity_.x = (dx > 0 ? 0.02f : -0.02f);
		}

	} else {
		// 攻撃モード
		float dx = targetPos.x - worldTransform_.translation_.x;
		float attackSpeed = 0.05f;
		if (std::fabs(dx) < 0.1f) {
			velocity_.x = 0.0f;
		} else {
			velocity_.x = (dx > 0 ? attackSpeed : -attackSpeed);
		}
	}

	// 左右の壁判定
	CollisionInfo info;
	info.move.x = velocity_.x * speedMultiplier_;
	info.move.y = 0.0f;
	CheckMapCollisionLeft(info);
	CheckMapCollisionRight(info);

	worldTransform_.translation_.x += info.move.x;
	worldTransform_.translation_.y = baseHeight_ + hoverOffset;

	// 回転演出
	if (healTimer_ > 0.0f) {
		healTimer_ -= 1.0f / 60.0f;
		worldTransform_.rotation_.y += 0.3f; // クルクル回る
	} else {
		if (target_) {
			float dx = target_->GetWorldTransform().translation_.x - worldTransform_.translation_.x;
			worldTransform_.rotation_.y = (dx > 0) ? std::numbers::pi_v<float> / 2.0f : std::numbers::pi_v<float> * 3.0f / 2.0f;
		}
	}
}

void FlyingEnemy::PerformUniqueAction(std::list<Enemy*>& enemies) {
	if (IsDead()) {
		return;
	}

	// 最も近い傷ついた仲間を探す
	float minDist = FLT_MAX;
	Enemy* bestCandidate = nullptr;

	for (Enemy* other : enemies) {
		// 自分自身や死んでいる敵は無視
		if (other == this || other->IsDead()) {
			continue;
		}

		// HPが満タンなら回復しない
		if (other->GetHp() >= other->GetMaxHp()) {
			continue;
		}

		// 距離をチェック
		float dist = Length(other->GetWorldTransform().translation_ - worldTransform_.translation_);
		if (dist < minDist) {
			minDist = dist;
			bestCandidate = other;
		}
	}

	healTarget_ = bestCandidate;

	// クールダウン中なら何もしない
	if (healTimer_ > 0.0f) {
		return;
	}

	// 射程圏内なら回復実行
	if (healTarget_ && minDist < kHealRange) {
		healTarget_->TakeDamage(-2); // マイナスのダメージ＝回復
		healTimer_ = kHealCooldown;
	}
}