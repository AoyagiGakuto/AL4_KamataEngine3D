#include <cassert>
#define NOMINMAX
#include "Player.h"
#include "Enemy.h"
#include "MapChipField.h"
#include "MyMath.h"
#include <algorithm>
#include <array>
#include <numbers>

using namespace KamataEngine;
using namespace MathUtility;

const float kJumpVelocity = 0.25f;
const float kGravity = 0.01f;
static inline const float kAttenuation = 0.005f;
static inline const float kLimitRunSpeed = 0.1f;
static inline const float kLimitFallSpeed = 0.1f;       // 落下速度の上限ｋabutteru
static inline const float kGlideGravityScale = 0.1f;   // 重力
static inline const float kLimitGlideFallSpeed = 0.02f; // 滑空中の最大落下速度
bool landing = false;

void Player::Initialize(Model* model, Camera* camera, const Vector3& position) {
	assert(model);
	model_ = model;
	worldTransform_.translation_ = position;
	camera_ = camera;
	worldTransform_.Initialize();
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	OnGround_ = true;
	maxHp_ = 100;
	hp_ = maxHp_;
	isDead_ = false;
}
void Player::Update() {
	if (knockbackTimer_ > 0.0f) {
		knockbackTimer_ -= 1.0f / 60.0f;
		// ノックバック中は入力を受け付けない
	} else {
		// タイマーが0のときだけ移動操作ができる
		InputMove();
	}

	// --- X方向のみ移動---
	CollisionMapInfo colX;
	colX.move = {velocity_.x, 0.0f, 0.0f};
	CollisionMapCheck(colX);
	worldTransform_.translation_ += colX.move;
	if (colX.isHitWall) {
		velocity_.x = 0;
	}

	// --- Y方向のみ移動---
	CollisionMapInfo colY;
	colY.move = {0.0f, velocity_.y, 0.0f};
	CollisionMapCheck(colY);
	worldTransform_.translation_ += colY.move;

	if (colY.isOnGround) {
		OnGround_ = true;
		jumpCount_ = 0;
		isGliding_ = false;
	} else {
		// gは重力
		const float g = kGravityAcceleration * (isGliding_ ? kGlideGravityScale : 1.0f);
		velocity_.y -= g;

		// 滑空中は落下上限も弱める
		const float fallLimit = isGliding_ ? -kLimitGlideFallSpeed : -kLimitFallSpeed;
		velocity_.y = std::max(velocity_.y, fallLimit);

		OnGround_ = false;
	}
	if (colY.isCeiling) {
		velocity_.y = 0;
	}

	if (isLockedOn_ && targetEnemy_) {
		// ロックオン中は敵の方向を向く
		Vector3 playerPos = worldTransform_.translation_;
		Vector3 targetPos = targetEnemy_->GetWorldTransform().translation_;
		float dx = targetPos.x - playerPos.x;

		// 向きを即座に変更
		if (dx > 0.0f) { // 敵が右にいる
			worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
			lrDirection_ = LRDirection::kRight;
		} else { // 敵が左にいる
			worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;
			lrDirection_ = LRDirection::kLeft;
		}
		turnTimer_ = 0.0f; // 振り向きアニメーションはキャンセル

	} else {
		// ロックオンしていない時は、通常のアニメーション
		AnimateTurn();
	}

	// K長押しチャージ処理
	if (isLockedOn_ && Input::GetInstance()->PushKey(DIK_K)) {
		// 既に準備完了でなければチャージ
		if (!isChargeAttackReady_) {
			chargeTimer_ += 1.0f / 60.0f;
			if (chargeTimer_ >= kChargeAttackTime) {
				isChargeAttackReady_ = true;
				chargeTimer_ = 0.0f;         // タイマーリセット
			}
		}
	} else {
		// Kを離したか、ロックオンが外れたらチャージリセット
		chargeTimer_ = 0.0f;
	}

	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}



void Player::Draw() {
	if (isDead_) {
		// 死亡後は描画しない
		return;
	}

	if (!model_ || !camera_) {
		return;
	}
	model_->Draw(worldTransform_, *camera_);
}

void Player::InputMove() {

	input = false;
	if (Input::GetInstance()->PushKey(DIK_A) || Input::GetInstance()->PushKey(DIK_D)) {
		input = true;
	}

	// 減速
	const float decelGround = 0.004f; // 地上は強く止める
	const float decelAir = 0.001f;    // 空中は弱め
	const float decel = OnGround_ ? decelGround : decelAir;

	if (!input) {
		if (velocity_.x > 0.0f) {
			velocity_.x = std::max(0.0f, velocity_.x - decel);
		} else if (velocity_.x < 0.0f) {
			velocity_.x = std::min(0.0f, velocity_.x + decel);
		}
		if (std::abs(velocity_.x) < 0.0005f) {
			velocity_.x = 0.0f;
		}
	}

	// 入力があるときだけ加速
	if (input) {
		Vector3 acceleration = {};

		if (Input::GetInstance()->PushKey(DIK_A)) {
			if (velocity_.x < 0.0f) {
				velocity_.x *= (1.0f - kAttenuation);
			}
			acceleration.x = -kAttenuation;

			if (lrDirection_ != LRDirection::kLeft && !isLockedOn_) {
				lrDirection_ = LRDirection::kLeft;
				turnFirstRotationY_ = worldTransform_.rotation_.y;
				turnTimer_ = kTimeTurn;
			}

		} else if (Input::GetInstance()->PushKey(DIK_D)) {
			if (velocity_.x > 0.0f) {
				velocity_.x *= (1.0f - kAttenuation);
			}
			acceleration.x = kAttenuation;

			if (lrDirection_ != LRDirection::kRight && !isLockedOn_) {
				lrDirection_ = LRDirection::kRight;
				turnFirstRotationY_ = worldTransform_.rotation_.y;
				turnTimer_ = kTimeTurn;
			}
		}

		velocity_ += acceleration;
		velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
	}

	// 二段ジャンプ
	if (Input::GetInstance()->TriggerKey(DIK_W)) {
		if (jumpCount_ < 2) {
			velocity_.y = kJumpVelocity;
			OnGround_ = false;
			landing = false;
			jumpCount_++;
		}
	}

	// 滑空
	if (!OnGround_ && velocity_.y <= 0.0f && Input::GetInstance()->PushKey(DIK_SPACE)) {
		isGliding_ = true;
	} else {
		isGliding_ = false;
	}
}

void Player::AnimateTurn() {
	if (turnTimer_ > 0.0f) {
		turnTimer_ -= 1.0f / 60.0f;
		if (turnTimer_ < 0.0f)
			turnTimer_ = 0.0f;
		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		float t = turnTimer_ / kTimeTurn;
		worldTransform_.rotation_.y = EaseInOut(destinationRotationY, turnFirstRotationY_, t);
		if (turnTimer_ <= 0.0f) {
			worldTransform_.rotation_.y = destinationRotationY;
		}
	}
}

void Player::Die() {
	isDead_ = true;
	velocity_ = {0.0f, 0.0f, 0.0f}; // 動きを止める
}

bool Player::IsDead() const { return isDead_; }

void Player::CollisionMapCheck(CollisionMapInfo& Info) {
	CheckMapCollisionDown(Info);
	CheckMapCollisionUp(Info);
	CheckMapCollisionLeft(Info);
	CheckMapCollisionRight(Info);
}

void Player::CheckMapCollision(CollisionMapInfo& Info) {
	CheckMapCollisionUp(Info);
	CheckMapCollisionDown(Info);
	CheckMapCollisionLeft(Info);
	CheckMapCollisionRight(Info);
}

void Player::CheckMapCollisionUp(CollisionMapInfo& Info) {
	if (Info.move.y <= 0.0f) {
		return;
	}
	Vector3 leftTop = CornerPosition(worldTransform_.translation_ + Info.move, kLeftTop);
	Vector3 rightTop = CornerPosition(worldTransform_.translation_ + Info.move, kRightTop);

	MapChipField::IndexSet leftIndex = mapChipField_->GetMapChipIndexSetByPosition(leftTop);
	MapChipField::IndexSet rightIndex = mapChipField_->GetMapChipIndexSetByPosition(rightTop);

	if (mapChipField_->GetMapChipTypeByIndex(leftIndex.xIndex, leftIndex.yIndex) == MapChipType::kBlock ||
	    mapChipField_->GetMapChipTypeByIndex(rightIndex.xIndex, rightIndex.yIndex) == MapChipType::kBlock) {
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(leftIndex.xIndex, leftIndex.yIndex);
		float ceilY = rect.bottom - kHeight / 2.0f - kBlank;
		Info.move.y = std::min(Info.move.y, ceilY - worldTransform_.translation_.y);
		Info.isCeiling = true;
	}
}

void Player::CheckMapCollisionDown(CollisionMapInfo& Info) {
	if (Info.move.y >= 0.0f)
		return;
	Vector3 leftBottom = CornerPosition(worldTransform_.translation_ + Info.move, kLeftBottom);
	Vector3 rightBottom = CornerPosition(worldTransform_.translation_ + Info.move, kRightBottom);

	MapChipField::IndexSet leftIndex = mapChipField_->GetMapChipIndexSetByPosition(leftBottom);
	MapChipField::IndexSet rightIndex = mapChipField_->GetMapChipIndexSetByPosition(rightBottom);

	if (mapChipField_->GetMapChipTypeByIndex(leftIndex.xIndex, leftIndex.yIndex) == MapChipType::kBlock ||
	    mapChipField_->GetMapChipTypeByIndex(rightIndex.xIndex, rightIndex.yIndex) == MapChipType::kBlock) {
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(leftIndex.xIndex, leftIndex.yIndex);
		float groundY = rect.top + kHeight / 2.0f + kBlank;
		Info.move.y = std::max(Info.move.y, groundY - worldTransform_.translation_.y);
		Info.isOnGround = true;
	}
}

void Player::CheckMapCollisionLeft(CollisionMapInfo& Info) {
	if (Info.move.x >= 0.0f)
		return;
	Vector3 leftTop = CornerPosition(worldTransform_.translation_ + Info.move, kLeftTop);
	Vector3 leftBottom = CornerPosition(worldTransform_.translation_ + Info.move, kLeftBottom);

	MapChipField::IndexSet topIndex = mapChipField_->GetMapChipIndexSetByPosition(leftTop);
	MapChipField::IndexSet bottomIndex = mapChipField_->GetMapChipIndexSetByPosition(leftBottom);

	if (mapChipField_->GetMapChipTypeByIndex(topIndex.xIndex, topIndex.yIndex) == MapChipType::kBlock ||
	    mapChipField_->GetMapChipTypeByIndex(bottomIndex.xIndex, bottomIndex.yIndex) == MapChipType::kBlock) {
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(topIndex.xIndex, topIndex.yIndex);
		float wallX = rect.right + kWidth / 2.0f + kBlank;
		Info.move.x = std::max(Info.move.x, wallX - worldTransform_.translation_.x);
		Info.isHitWall = true;
	}
}

void Player::CheckMapCollisionRight(CollisionMapInfo& Info) {
	if (Info.move.x <= 0.0f)
		return;
	Vector3 rightTop = CornerPosition(worldTransform_.translation_ + Info.move, kRightTop);
	Vector3 rightBottom = CornerPosition(worldTransform_.translation_ + Info.move, kRightBottom);

	MapChipField::IndexSet topIndex = mapChipField_->GetMapChipIndexSetByPosition(rightTop);
	MapChipField::IndexSet bottomIndex = mapChipField_->GetMapChipIndexSetByPosition(rightBottom);

	if (mapChipField_->GetMapChipTypeByIndex(topIndex.xIndex, topIndex.yIndex) == MapChipType::kBlock ||
	    mapChipField_->GetMapChipTypeByIndex(bottomIndex.xIndex, bottomIndex.yIndex) == MapChipType::kBlock) {
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(topIndex.xIndex, topIndex.yIndex);
		float wallX = rect.left - kWidth / 2.0f - kBlank;
		Info.move.x = std::min(Info.move.x, wallX - worldTransform_.translation_.x);
		Info.isHitWall = true;
	}
}

AABB Player::GetAABB() {
	AABB aabb;

	aabb.min = {
	    worldTransform_.translation_.x - Player::kWidth / 2.0f + Player::kBlank, worldTransform_.translation_.y - Player::kHeight / 2.0f + Player::kBlank,
	    worldTransform_.translation_.z - Player::kWidth / 2.0f + Player::kBlank};

	aabb.max = {
	    worldTransform_.translation_.x + Player::kWidth / 2.0f - Player::kBlank, worldTransform_.translation_.y + Player::kHeight / 2.0f - Player::kBlank,
	    worldTransform_.translation_.z + Player::kWidth / 2.0f - Player::kBlank};

	return aabb;
}

Vector3 Player::GetWorldPosition() {
	Vector3 worldPos;
	worldPos.x = worldTransform_.translation_.x;
	worldPos.y = worldTransform_.translation_.y + Player::kHeight / 2.0f; // プレイヤーの中心位置を考慮
	worldPos.z = worldTransform_.translation_.z;
	return worldPos;
}


void Player::WarpTo(const Vector3& position) {
	// 位置だけ瞬間移動させて行列を更新
	worldTransform_.translation_ = position;
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

Vector3 CornerPosition(const Vector3& center, Corner corner) {
	switch (corner) {
	case kRightBottom:
		return center + Vector3{+Player::kWidth / 2.0f, -Player::kHeight / 2.0f, 0};
	case kLeftBottom:
		return center + Vector3{-Player::kWidth / 2.0f, -Player::kHeight / 2.0f, 0};
	case kRightTop:
		return center + Vector3{+Player::kWidth / 2.0f, +Player::kHeight / 2.0f, 0};
	case kLeftTop:
		return center + Vector3{-Player::kWidth / 2.0f, +Player::kHeight / 2.0f, 0};
	default:
		return center;
	}
}

void Player::LockOn(Enemy* target) {
	targetEnemy_ = target;
	isLockedOn_ = (target != nullptr);
}

void Player::LockOff() {
	targetEnemy_ = nullptr;
	isLockedOn_ = false;
}

void Player::TakeDamage(int damage) {
	hp_ -= damage;

	// HPが0以下になったら死亡
	if (hp_ <= 0) {
		hp_ = 0;
		Die(); // 死亡処理を実行
	}
}

void Player::Knockback(const Vector3& dir) {
	// 既にノックバック中なら無視（連続ヒット防止）
	if (knockbackTimer_ > 0.0f)
		return;

	// 弾き飛ばす速度
	float knockbackSpeed = 0.05f;
	float jumpPower = 0.2f; // 少し浮き上がる（ノックアップ）

	velocity_.x = dir.x * knockbackSpeed;
	velocity_.y = jumpPower;

	knockbackTimer_ = 0.5f; // 0.5秒間は操作不能＆ノックバック状態
}