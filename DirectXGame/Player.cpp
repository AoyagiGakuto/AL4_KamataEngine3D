#define NOMINMAX
#include "Player.h"
#include "MyMath.h"
#include <algorithm>
#include <numbers>

using namespace KamataEngine;
using namespace MathUtility;

// ジャンプ関連定数
const float kJumpVelocity = 0.25f;                // ジャンプ初速
const float kGravity = 0.01f;                     // 重力加速度
static inline const float kAttenuation = 0.005f;  // 横移動加速度
static inline const float kLimitRunSpeed = 0.1f;  // 横移動の限界速度
static inline const float kLimitFallSpeed = 0.2f; // 落下速度の限界値
bool landing = false;                             // 着地フラグ

void Player::Initialize(Model* model, Camera* camera, const Vector3& position) {
	assert(model);
	model_ = model;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	camera_ = camera;
	OnGround_ = true; // 地面にいる状態で初期化
}

void Player::Update() {
	InputMove();

	if (OnGround_) {
		// ジャンプ入力
		if (Input::GetInstance()->PushKey(DIK_UP)) {
			velocity_.y = kJumpVelocity;
			OnGround_ = false;
			landing = false;
		}
	}

	// 重力適用
	if (!OnGround_) {
		velocity_.y -= kGravity;
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}

	// 着地判定
	if (worldTransform_.translation_.y + velocity_.y <= 1.0f) {
		worldTransform_.translation_.y = 1.0f;
		velocity_.y = 0.0f;
		OnGround_ = true;
		landing = true;
	} else {
		worldTransform_.translation_.y += velocity_.y;
		landing = false;
	}

	// 横移動
	worldTransform_.translation_.x += velocity_.x;

	// 方向転換
	if (turnTimer_ > 0.0f) {
		turnTimer_ -= 1.0f / 60.0f;
		float destinationRotationYTable[] = {
		    std::numbers::pi_v<float> / 2.0f,       // kRight
		    std::numbers::pi_v<float> * 3.0f / 2.0f // kLeft
		};
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		worldTransform_.rotation_.y = EaseInOut(destinationRotationY, turnFirstRotationY_, turnTimer_ / kTimeTurn);
	}

	// アフィン変換行列の作成
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

void Player::Draw() { model_->Draw(worldTransform_, *camera_); }

void Player::InputMove() {
	if (OnGround_) {
		if (Input::GetInstance()->PushKey(DIK_LEFT) || Input::GetInstance()->PushKey(DIK_RIGHT)) {
			Vector3 acceleration = {};
			if (Input::GetInstance()->PushKey(DIK_LEFT)) {
				if (velocity_.x < 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x = -kAttenuation; // 左移動
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
			} else if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
				if (velocity_.x > 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x = kAttenuation; // 右移動
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
			}
			velocity_ += acceleration;
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		}
	} else {
		velocity_ = Vector3(0, -kGravityAcceleration, 0);
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}
}

void Player::CollisionMapCheck(CollisionMapInfo& Info) {
	CollisionMapInfo collisionMapInfo;
	collisionMapInfo.move = velocity_;
	//	マップ衝突チェック
	CheckMapCollision(collisionMapInfo);
	// マップ衝突判定上方向
	CheckMapCollisionUp(info);
	// マップ衝突判定下方向
	CheckMapCollisionDown(info);
	// マップ衝突判定左方向
	CheckMapCollisionLeft(info);
	// マップ衝突判定右方向
	CheckMapCollisionRight(info);
}

// p14やってる
void Player::CheckMapCollisionUp(CollisionMapInfo& Info) {
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < kNumCorner; ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + Info.move, static_cast<Corner>(i));
	}
	if (info.move.y <= 0.0f) {
		return; // 上方向の移動ではない
	}
}

void Player::CheckMapCollisionDown(CollisionMapInfo& Info) {
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < kNumCorner; ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + Info.move, static_cast<Corner>(i));
	}
}

void Player::CheckMapCollisionLeft(CollisionMapInfo& Info) {
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < kNumCorner; ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + Info.move, static_cast<Corner>(i));
	}
}

void Player::CheckMapCollisionRight(CollisionMapInfo& Info) {
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < kNumCorner; ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + Info.move, static_cast<Corner>(i));
	}
}

Vector3 CornerPosition(const Vector3& center, Corner corner) {
	if (corner == kRightBottom) {
		return center + {+kWidth / 2.0f, -kHeight / 2.0f, 0};
	}

	else if (corner == kLeftBottom) {
		return center + {-kWidth / 2.0f, -kHeight / 2.0f, 0};
	}

	else if (corner == kRightTop) {
		return center + {+kWidth / 2.0f, +kHeight / 2.0f, 0};
	} else {
		return center + {-kWidth / 2.0f, +kHeight / 2.0f, 0};
	}
	Vector3 offsetTable[kNumCorner] = {
		{+kWidth / 2.0f, -kHeight / 2.0f, 0}, // kRightBottom
		{-kWidth / 2.0f, -kHeight / 2.0f, 0}, // kLeftBottom
		{+kWidth / 2.0f, +kHeight / 2.0f, 0}, // kRightTop
		{-kWidth / 2.0f, +kHeight / 2.0f, 0}  // kLeftTop
	};

	return center + offsetTable[static_cast<uint32_t>(corner)];
}