#include "Skydome.h"

void Skydome::Initialize() {
	model_ = model;
	camera_ = camera;
}

void Skydome::Update() {}

void Skydome::Draw() {
	if (model_ && camera_) {
		model_->Draw(worldTransform_, *camera_);
	}
}
