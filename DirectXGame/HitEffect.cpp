#include "HitEffect.h"
#include <string>
#include <algorithm>

HitEffect::~HitEffect() {
	for (WorldTransform* wt : transforms_) {
		delete wt;
	}
	transforms_.clear();
}

void HitEffect::Initialize(std::array<Model*, 10>& numberModels, Camera* camera, const Vector3& pos, int number) {

	camera_ = camera;
	alive_ = true;
	lifetime_ = 0.75f;
	maxLifetime_ = 0.75f;
	scale_ = 0.5f;
	yOffset_ = 0.0f;
	basePos_ = pos;

	color_ = {1.0f, 1.0f, 1.0f, 1.0f};
	objectColor_.Initialize();
	objectColor_.SetColor(color_);

	numStr_ = std::to_string(number);

	for (char c : numStr_) {
		int digit = c - '0';

		if (digit < 0 || digit > 9) {
			modelsToDraw_.push_back(nullptr);
		} else {
			modelsToDraw_.push_back(numberModels[digit]);
		}

		WorldTransform* wt = new WorldTransform();
		wt->Initialize();
		transforms_.push_back(wt);
	}

	UpdatePosition(pos);
}

void HitEffect::UpdatePosition(const Vector3& basePos) {

	basePos_ = basePos;

	float totalWidth = numStr_.length() * kDigitWidth * scale_;
	float currentX = basePos_.x - (totalWidth / 2.0f) + (kDigitWidth * scale_ / 2.0f);
	float currentY = basePos_.y + yOffset_;

	for (size_t i = 0; i < transforms_.size(); ++i) {
		transforms_[i]->scale_ = {scale_, scale_, scale_};
		transforms_[i]->translation_ = {currentX, currentY, basePos_.z};

		transforms_[i]->matWorld_ = MakeAffineMatrix(transforms_[i]->scale_, transforms_[i]->rotation_, transforms_[i]->translation_);
		transforms_[i]->TransferMatrix();

		currentX += kDigitWidth * scale_;
	}
}

void HitEffect::Update() {
	if (!alive_)
		return;

	lifetime_ -= 1.0f / 60.0f;
	if (lifetime_ <= 0.0f) {
		alive_ = false;
		return;
	}

	float lifeRatio = lifetime_ / maxLifetime_;
	float alpha = std::clamp(lifeRatio, 0.0f, 1.0f);

	// フェードアウト（α値を減らす）
	color_.w = alpha;
	objectColor_.SetColor(color_);

	yOffset_ += speed_;
}

void HitEffect::Draw() {
	if (!alive_ || !camera_)
		return;

	for (size_t i = 0; i < modelsToDraw_.size(); ++i) {
		if (modelsToDraw_[i]) {
			modelsToDraw_[i]->Draw(*transforms_[i], *camera_, &objectColor_);
		}
	}
}