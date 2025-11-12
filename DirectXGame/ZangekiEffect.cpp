#include "ZangekiEffect.h"
#include <cstdlib>
#include <numbers>

ZangekiEffect::~ZangekiEffect() {}

void ZangekiEffect::Initialize(Model* model, Camera* camera, const Vector3& pos) {
	model_ = model;
	camera_ = camera;
	transform_.Initialize();
	transform_.translation_ = pos;
	transform_.scale_ = {0.5f, 0.5f, 0.5f}; // 少し小さめに開始
	alive_ = true;
	lifetime_ = maxLifetime_;
	objectColor_.Initialize();
	objectColor_.SetColor(color_);
}

void ZangekiEffect::SetRandomRotation() {
	float kPi = std::numbers::pi_v<float>;
	// X, Y, Z 軸にランダムな回転を与える
	transform_.rotation_.x = ((float)(rand() % 1000) / 999.0f) * 2.0f * kPi;
	transform_.rotation_.y = ((float)(rand() % 1000) / 999.0f) * 2.0f * kPi;
	transform_.rotation_.z = ((float)(rand() % 1000) / 999.0f) * 2.0f * kPi;
}

void ZangekiEffect::Update() {
	if (!alive_)
		return;

	lifetime_ -= 1.0f / 60.0f;
	if (lifetime_ <= 0.0f) {
		alive_ = false;
		return;
	}

	// 0から1 (開始時) -> 0 (終了時)
	float t = lifetime_ / maxLifetime_;

	// フェードアウト
	color_.w = t;
	objectColor_.SetColor(color_);

	// 少しずつ拡大しながら消える
	float scale = (1.0f - t) * 1.5f + 0.5f; // 0.5 から 2.0 へ拡大
	transform_.scale_ = {scale, scale, scale};

	// 行列更新
	transform_.matWorld_ = MakeAffineMatrix(transform_.scale_, transform_.rotation_, transform_.translation_);
	transform_.TransferMatrix();
}

void ZangekiEffect::Draw() {
	if (alive_ && model_ && camera_) {
		model_->Draw(transform_, *camera_, &objectColor_);
	}
}