#include "CameraController.h"
#include "Player.h"
#include "GameScene.h"

using namespace KamataEngine;

void CameraController::Initialize() { camera_.Initialize(); }

void CameraController::Update() {
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	camera_.translation_ = targetWorldTransform.translation_ + targetOffset_;
	camera_.translation_.x = max(camera_.translation_.x , movebleArea_.left);
	camera_.translation_.x = min(camera_.translation_.x, movebleArea_.right);
	camera_.translation_.y = max(camera_.translation_.y, movebleArea_.bottom);
	camera_.translation_.y = min(camera_.translation_.y, movebleArea_.top);
	camera_.UpdateMatrix();
}

void CameraController::Reset() {
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	camera_.translation_ = targetWorldTransform.translation_ + targetOffset_;
}