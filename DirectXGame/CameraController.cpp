#include "CameraController.h"
#include "Player.h"

using namespace KamataEngine;

void CameraController::Initialize() {
	camera_.translation_ = target_->GetWorldTransform().translation_ + targetOffset_;
	camera_.UpdateMatrix();
}

void CameraController::Update() {
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	camera_.translation_ = targetWorldTransform.translation_ + targetOffset_;
	camera_.UpdateMatrix();
}

void CameraController::Reset() {
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	camera_.translation_ = targetWorldTransform.translation_ + targetOffset_;
}