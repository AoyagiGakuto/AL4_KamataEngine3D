#define NOMINMAX
#include "Player.h"
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
static inline const float kLimitFallSpeed = 0.2f;
bool landing = false;

void Player::Initialize(Model* model, Camera* camera, const Vector3& position) {
	assert(model);
	model_ = model;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	camera_ = camera;
	OnGround_ = true;
}
void Player::Update() {
	InputMove();

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
	} else {
		velocity_.y -= kGravityAcceleration;
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
		OnGround_ = false;
	}
	if (colY.isCeiling) {
		velocity_.y = 0;
	}

	AnimateTurn();
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
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
				acceleration.x = -kAttenuation;
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
			} else if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
				if (velocity_.x > 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x = kAttenuation;
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
			}
			velocity_ += acceleration;
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		}
		if (Input::GetInstance()->PushKey(DIK_UP)) {
			velocity_.y = kJumpVelocity;
			OnGround_ = false;
			landing = false;
		}
	}
}

void Player::AnimateTurn() {
	if (turnTimer_ > 0.0f) {
		turnTimer_ -= 1.0f / 60.0f;
		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		worldTransform_.rotation_.y = EaseInOut(destinationRotationY, turnFirstRotationY_, turnTimer_ / kTimeTurn);
	}
}

void Player::CollisionMapCheck(CollisionMapInfo& Info) {
	CheckMapCollisionDown(Info);
	CheckMapCollisionUp(Info);
	CheckMapCollisionLeft(Info);
	CheckMapCollisionRight(Info);
}

void Player::CheckMapCollision(CollisionMapInfo& Info) {
	// 必要に応じて個別の方向だけ呼び出してもOK
	CheckMapCollisionUp(Info);
	CheckMapCollisionDown(Info);
	CheckMapCollisionLeft(Info);
	CheckMapCollisionRight(Info);
}

void Player::CheckMapCollisionUp(CollisionMapInfo& Info) {
	if (Info.move.y <= 0.0f)
		return;
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