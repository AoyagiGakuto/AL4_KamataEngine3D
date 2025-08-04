#include "CameraController.h"
#include "GameScene.h"
#include "Player.h"

using namespace KamataEngine;

void CameraController::Initialize() {
	camera_.Initialize();
	Rect area;
	area.left = 12.0f;
	area.right = 88.0f;
	area.bottom = 6.0f;
	area.top = 20.0f;
	SetMovableArea(area);
}

void CameraController::Update() {
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	// 追尾対象とオフセットからカメラの目標座標を計算
	Vector3 targetVelocity = target_->GetVelocity();
	targetPosition_ = targetWorldTransform.translation_ + targetOffset_ + targetVelocity * kVelocityBias;
	// 座標補間によりゆったり追従
	camera_.translation_.x = Lerp(camera_.translation_.x, targetPosition_.x, kInterpolationRate);

	camera_.translation_.x = max(camera_.translation_.x, movebleArea_.left);
	camera_.translation_.x = min(camera_.translation_.x, movebleArea_.right);
	camera_.translation_.y = max(camera_.translation_.y, movebleArea_.bottom);
	camera_.translation_.y = min(camera_.translation_.y, movebleArea_.top);

	/*
	camera_.translation_.x = max(camera_.translation_.x, targetPosition_.x + targetMargin.left);
	camera_.translation_.x = min(camera_.translation_.x, targetPosition_.x + targetMargin.right);
	camera_.translation_.y = max(camera_.translation_.y, targetPosition_.y + targetMargin.bottom);
	camera_.translation_.y = min(camera_.translation_.y, targetPosition_.y + targetMargin.top);
	*/

	camera_.UpdateMatrix();
}

void CameraController::Reset() {
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	camera_.translation_ = targetWorldTransform.translation_ + targetOffset_;
}