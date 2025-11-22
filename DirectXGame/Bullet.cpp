#include "Bullet.h"
#include <algorithm>

void Bullet::Initialize(Model* model, Camera* camera, const Vector3& pos, const Vector3& dir) {
	model_ = model;
	camera_ = camera;
	transform_.Initialize();
	transform_.translation_ = pos;
	transform_.scale_ = {0.2f, 0.2f, 0.2f};
	direction_ = dir;
	float len = std::sqrt(direction_.x * direction_.x + direction_.y * direction_.y + direction_.z * direction_.z);
	if (len > 0.0001f) {
		direction_.x /= len;
		direction_.y /= len;
		direction_.z /= len;
	}
	alive_ = true;
	lifetime_ = 2.0f;
}

void Bullet::Update() {
	if (!alive_) {
		return;
	}
	transform_.translation_.x += direction_.x * speed_;
	transform_.translation_.y += direction_.y * speed_;
	transform_.translation_.z += direction_.z * speed_;

	lifetime_ -= 1.0f / 60.0f;
	if (lifetime_ <= 0.0f) {
		alive_ = false;
	}

	transform_.matWorld_ = MakeAffineMatrix(transform_.scale_, transform_.rotation_, transform_.translation_);
	transform_.TransferMatrix();
}

void Bullet::Draw() {
	if (!alive_ || !model_ || !camera_) {
		return;
	}
	model_->Draw(transform_, *camera_);
}

AABB Bullet::GetAABB() const {
	AABB aabb;
	const float half = 0.2f * 0.5f;
	aabb.min = {transform_.translation_.x - half, transform_.translation_.y - half, transform_.translation_.z - half};
	aabb.max = {transform_.translation_.x + half, transform_.translation_.y + half, transform_.translation_.z + half};
	return aabb;
}
