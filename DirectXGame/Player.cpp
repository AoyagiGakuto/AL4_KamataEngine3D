#include "Player.h"
#include "MyMath.h"
#include <algorithm>
#include <numbers>

using namespace KamataEngine;
using namespace MathUtility;

// ジャンプ関連定数
const float kJumpVelocity = 0.25f;               // ジャンプ初速
const float kGravity = 0.01f;                    // 重力加速度
static inline const float kAttenuation = 0.005f; // 横移動加速度
static inline const float kLimitRunSpeed = 0.1f; // 横移動の限界速度

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
	if (OnGround_)
	// 横移動入力
	if (Input::GetInstance()->PushKey(DIK_LEFT) || Input::GetInstance()->PushKey(DIK_RIGHT)) {
		Vector3 acceleration = {};
		if (Input::GetInstance()->PushKey(DIK_LEFT)) {
			if (velocity_.z < 0.0f) {
				velocity_.z *= (1.0f - kAttenuation); // 左移動中に右キーを押した場合、速度をリセット
			}
			acceleration.z = -kAcceleration; // 左移動
			if (lrDirection_ != LRDirection::kRight) {
				lrDirection_ = LRDirection::kRight;
				turnFirstRotation_ = worldTransform_.rotation_.y; // 初回の方向転換時に現在のY軸回転を記録
				turnTimer_ = kTimeTurn;                           // 初回の方向転換時にターン時間を設定
			}
		} else if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
			if (velocity_.z > 0.0f) {
				velocity_.z *= (1.0f - kAttenuation); // 右移動中に左キーを押した場合、速度をリセット
			}
			acceleration.z = kAcceleration; // 右移動
			if (lrDirection_ != LRDirection::kLeft) {
				lrDirection_ = LRDirection::kLeft;
				turnFirstRotation_ = worldTransform_.rotation_.y; // 初回の方向転換時に現在のY軸回転を記録
				turnTimer_ = kTimeTurn;                           // 初回の方向転換時にターン時間を設定
			}
		}
		velocity_ += acceleration;
		velocity_.z = std::clamp(velocity_.z, -kLimitRunSpeed, kLimitRunSpeed); // 横移動の限界速度を適用
	} else {
		velocity_.z *= (1.0f - kAttenuation); // 摩擦による減速
	}
	else {
		velocity_ += Vector3(0, -kGravity, 0); // 重力を適用
		velocity_.y = std::max(velocity_.y - kGravity, -kJumpVelocity); // ジャンプの初速を制限
	}

	if (turnTimer_ > 0.0f) {

		// タイマーを1/60だけカウントダウン
		turnTimer_ -= 1.0f / 60.0f;

		float destinationRotationYTable[] = {
		    std::numbers::pi_v<float> / 2.0f,       // kRight
		    std::numbers::pi_v<float> * 3.0f / 2.0f // kLeft
		};
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		worldTransform_.rotation_.y = destinationRotationY; // 方向転換
	}

	// 移動
	worldTransform_.translation_ += velocity_;

	// アフィン変換行列の作成
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

void Player::Draw() { model_->Draw(worldTransform_, *camera_); }