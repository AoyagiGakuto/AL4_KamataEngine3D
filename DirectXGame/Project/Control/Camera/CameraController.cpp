#include "CameraController.h"
#include "GameScene.h"
#include "Player.h"

using namespace KamataEngine;
using namespace KamataEngine::MathUtility;

// ==========================================================================
// 初期化処理
// ==========================================================================

void CameraController::Initialize() {
	camera_.Initialize();

	// デフォルトの移動範囲設定
	Rect area;
	area.left = 12.0f;
	area.right = 88.0f;
	area.bottom = 6.0f;
	area.top = 20.0f;
	SetMovableArea(area);
}

// ==========================================================================
// 更新処理
// ==========================================================================

void CameraController::Update() {
	
	// 追尾対象のワールド変換行列を取得
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	
	// 追尾対象とオフセットからカメラの目標座標を計算
	Vector3 targetVelocity = target_->GetVelocity();
	
	// 速度分を加味して予測位置を計算
	targetPosition_ = targetWorldTransform.translation_ + targetOffset_ + targetVelocity * kVelocityBias;
	
	// 座標補間によりゆったり追従
	camera_.translation_.x = Lerp(camera_.translation_.x, targetPosition_.x, kInterpolationRate);

	// シェイク加算
	if (shakeTimer_ > 0.0f) {
		shakeTimer_ -= 1.0f / 60.0f;

		// ランダムにずらす
		float randX = ((float)rand() / RAND_MAX - 0.5f) * shakeMagnitude_;
		float randY = ((float)rand() / RAND_MAX - 0.5f) * shakeMagnitude_;

		// カメラの目標位置（または実際の座標）に足す
		camera_.translation_.x += randX;
		camera_.translation_.y += randY;
	}

	// カメラが移動可能エリアの外に出ないように
	camera_.translation_.x = max(camera_.translation_.x, movableArea_.left);
	camera_.translation_.x = min(camera_.translation_.x, movableArea_.right);
	
	camera_.translation_.y = max(camera_.translation_.y, movableArea_.bottom);
	camera_.translation_.y = min(camera_.translation_.y, movableArea_.top);

	// 行列更新
	camera_.UpdateMatrix();
}

// ==========================================================================
// リセット処理
// ==========================================================================

void CameraController::Reset() {
	
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	
	// 直接座標設定
	camera_.translation_ = targetWorldTransform.translation_ + targetOffset_;
}

// ==========================================================================
// シェイク処理
// ==========================================================================

void CameraController::Shake(float magnitude, float duration) {
	shakeMagnitude_ = magnitude;
	shakeTimer_ = duration;
}