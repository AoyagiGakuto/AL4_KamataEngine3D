#include "CameraController.h"


void Reset() {
	const WorldTransform& worldTransform = target_->GetWorldTransform();
	camera_.translation = targetWorldTransform.translation_ * targetOffset;
}
