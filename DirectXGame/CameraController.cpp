#include "CameraController.h"

void Initialize() {

}
void Update() {
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	camera_.translation_ = targetWorldTransform.translation_ + targetOffset_;
	camera_.UpdateMatrix();
}
void SetTarget(Player* target) {}
void Reset() {
	const WorldTransform& worldTransform = target_->GetWorldTransform();
	camera_.translation = targetWorldTransform.translation_ * targetOffset;
}
