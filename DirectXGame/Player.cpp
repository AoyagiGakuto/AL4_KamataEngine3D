#define NOMINMAX
#include "Player.h"
#include "MapChipField.h"
#include "MyMath.h"
#include <algorithm>
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
	if (OnGround_) {
		if (Input::GetInstance()->PushKey(DIK_UP)) {
			velocity_.y = kJumpVelocity;
			OnGround_ = false;
			landing = false;
		}
	}
	if (!OnGround_) {
		velocity_.y -= kGravity;
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}
	if (worldTransform_.translation_.y + velocity_.y <= 1.0f) {
		worldTransform_.translation_.y = 1.0f;
		velocity_.y = 0.0f;
		OnGround_ = true;
		landing = true;
	} else {
		worldTransform_.translation_.y += velocity_.y;
		landing = false;
	}
	worldTransform_.translation_.x += velocity_.x;
	if (turnTimer_ > 0.0f) {
		turnTimer_ -= 1.0f / 60.0f;
		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		worldTransform_.rotation_.y = EaseInOut(destinationRotationY, turnFirstRotationY_, turnTimer_ / kTimeTurn);
	}
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
	} else {
		velocity_ = Vector3(0, -kGravityAcceleration, 0);
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}
}

void Player::CollisionMapCheck(CollisionMapInfo& Info) {
	CollisionMapInfo collisionMapInfo;
	collisionMapInfo.move = velocity_;
	CheckMapCollision(collisionMapInfo);
	CheckMapCollisionUp(Info);
	//CheckMapCollisionDown(Info);
	//CheckMapCollisionLeft(Info);
	//CheckMapCollisionRight(Info);
}

void Player::CheckMapCollisionUp(CollisionMapInfo& Info) {
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < kNumCorner; ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + Info.move, static_cast<Corner>(i));
	}
	if (Info.move.y <= 0.0f) {
		return;
	}
	MapChipType mapChipType;
	bool hit = false;
	IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	if (hit) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, +kHeight / 2.0f, 0));
		Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		Info.move.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));
		Info.isCollision = true;
	}
}
/*
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
*/
void Player::CheckMapCollision(CollisionMapInfo& Info) {
	(void)Info;
	// 今後実装予定の場合、このようにしておくと良い
}

Vector3 CornerPosition(const Vector3& center, Corner corner) {
	if (corner == kRightBottom) {
		return center + Vector3{+Player::kWidth / 2.0f, -Player::kHeight / 2.0f, 0};
	} else if (corner == kLeftBottom) {
		return center + Vector3{-Player::kWidth / 2.0f, -Player::kHeight / 2.0f, 0};
	} else if (corner == kRightTop) {
		return center + Vector3{+Player::kWidth / 2.0f, +Player::kHeight / 2.0f, 0};
	} else {
		return center + Vector3{-Player::kWidth / 2.0f, +Player::kHeight / 2.0f, 0};
	}
}