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
	if (OnGround_) {
		// 横移動入力
		if (Input::GetInstance()->PushKey(DIK_LEFT) || Input::GetInstance()->PushKey(DIK_RIGHT)) {
			Vector3 acceleration = {};
			if (Input::GetInstance()->PushKey(DIK_LEFT)) {
				if (velocity_.z < 0.0f) {
					velocity_.z *= (1.0f - kAttenuation);
				}
				acceleration.z = -kAttenuation; // 左移動
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
			} else if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
				if (velocity_.z > 0.0f) {
					velocity_.z *= (1.0f - kAttenuation);
				}
				acceleration.z = kAttenuation; // 右移動
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
			}
			velocity_ += acceleration;
			velocity_.z = std::clamp(velocity_.z, -kLimitRunSpeed, kLimitRunSpeed);
		} else {
			velocity_.z *= (1.0f - kAttenuation); // 慣性減衰
		}

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
	worldTransform_.translation_.z += velocity_.z;

	// 方向転換
	if (turnTimer_ > 0.0f) {
		turnTimer_ -= 1.0f / 60.0f;
		float destinationRotationYTable[] = {
		    std::numbers::pi_v<float> / 2.0f,       // kRight
		    std::numbers::pi_v<float> * 3.0f / 2.0f // kLeft
		};
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		worldTransform_.rotation_.y = EaseInOut(destinationRotationY,turnFirstRotationY_,turnTimer_/kTimeTurn);
	}

	// アフィン変換行列の作成
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

void Player::Draw() { model_->Draw(worldTransform_, *camera_); }