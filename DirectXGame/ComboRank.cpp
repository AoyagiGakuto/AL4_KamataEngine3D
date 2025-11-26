#include "ComboRank.h"
#include "MyMath.h"
#include <algorithm>

using namespace KamataEngine;

ComboRank::~ComboRank() {
	for (Model* m : models_) {
		delete m;
	}
	models_.fill(nullptr);
}

void ComboRank::Initialize(Camera* uiCamera, const Vector3& pos) {
	uiCamera_ = uiCamera;

	wt_.Initialize();
	wt_.translation_ = pos;
	wt_.scale_ = {1.5f, 1.5f, 1.0f};

	LoadModels();
}

void ComboRank::LoadModels() {
	models_.fill(nullptr);
	models_[static_cast<size_t>(Rank::D)] = Model::CreateFromOBJ("rankD");
	models_[static_cast<size_t>(Rank::C)] = Model::CreateFromOBJ("rankC");
	models_[static_cast<size_t>(Rank::B)] = Model::CreateFromOBJ("rankB");
	models_[static_cast<size_t>(Rank::A)] = Model::CreateFromOBJ("rankA");
	models_[static_cast<size_t>(Rank::S)] = Model::CreateFromOBJ("rankS");
}

void ComboRank::AddHit(float value) {
	comboPoint_ += value;
	comboPoint_ = std::clamp(comboPoint_, 0.0f, kMaxPoint);

	// 何か当てたので時間リセット
	decayTimer_ = 0.0f;

	UpdateRank();
}

void ComboRank::OnEnemyKilled(float bonus) {
	comboPoint_ += bonus;
	comboPoint_ = std::clamp(comboPoint_, 0.0f, kMaxPoint);
	decayTimer_ = 0.0f;
	UpdateRank();
}

void ComboRank::OnPlayerDamaged() {
	// 被弾したら減らす
	comboPoint_ *= 0.4f;
	if (comboPoint_ < 0.0f) {
		comboPoint_ = 0.0f;
	}
	decayTimer_ = 0.0f;
	UpdateRank();
}

void ComboRank::Reset() {
	comboPoint_ = 0.0f;
	decayTimer_ = 0.0f;
	rank_ = Rank::None;
}

void ComboRank::UpdateRank() {
	Rank newRank = Rank::None;

	if (comboPoint_ >= 80.0f) {
		newRank = Rank::S;
	} else if (comboPoint_ >= 60.0f) {
		newRank = Rank::A;
	} else if (comboPoint_ >= 40.0f) {
		newRank = Rank::B;
	} else if (comboPoint_ >= 20.0f) {
		newRank = Rank::C;
	} else if (comboPoint_ >= 5.0f) {
		newRank = Rank::D;
	} else {
		newRank = Rank::None;
	}

	rank_ = newRank;
}

void ComboRank::Update(float deltaTime) {
	// 時間経過でコンボポイントを減衰させる
	decayTimer_ += deltaTime;

	if (decayTimer_ > kStartDecayTime && comboPoint_ > 0.0f) {
		comboPoint_ -= kDecaySpeed * deltaTime;
		if (comboPoint_ < 0.0f) {
			comboPoint_ = 0.0f;
		}
		UpdateRank();
	}

	// 行列更新
	wt_.matWorld_ = MakeAffineMatrix(wt_.scale_, wt_.rotation_, wt_.translation_);
	wt_.TransferMatrix();
}

void ComboRank::Draw() {
	if (!uiCamera_) {
		return;
	}
	if (rank_ == Rank::None) {
		return;
	}

	size_t idx = static_cast<size_t>(rank_);
	if (idx >= models_.size()) {
		return;
	}

	Model* model = models_[idx];
	if (!model) {
		return;
	}

	model->Draw(wt_, *uiCamera_);
}
