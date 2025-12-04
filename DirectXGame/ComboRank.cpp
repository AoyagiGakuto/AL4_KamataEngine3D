#include "ComboRank.h"
#include "MyMath.h"
#include <algorithm>
#include <numbers>

using namespace KamataEngine;

ComboRank::~ComboRank() {
	
	// モデル解放
	for (Model* m : models_) {
		delete m;
	}

	// 配列クリア
	models_.fill(nullptr);
}

// ==========================================================================
// 初期化とロード
// ==========================================================================

void ComboRank::Initialize(Camera* uiCamera, const Vector3& pos, Model* modelBar, Model* modelGauge) { // カメラセット
	uiCamera_ = uiCamera;

	// ワールド変換初期化
	wt_.Initialize();
	// 位置セット
	wt_.translation_ = pos;
	// スケールセット
	wt_.scale_ = {1.5f, 1.5f, 1.0f};

	wt_.rotation_.y = std::numbers::pi_v<float>;

	// ゲージ用モデルのセット
	modelBar_ = modelBar;
	modelGauge_ = modelGauge;

	// ゲージ用ワールド変換初期化
	wtBar_.Initialize();
	wtGauge_.Initialize();

	// ゲージの基本スケール設定
	wtBar_.scale_ = kGaugeScale;
	wtGauge_.scale_ = kGaugeScale;

	// ゲージの位置設定（ランク文字の少し下）
	Vector3 gaugePos = pos;
	gaugePos.y += kGaugeOffsetY;

	wtBar_.translation_ = gaugePos;
	wtGauge_.translation_ = gaugePos;

	// モデル読み込み
	LoadModels();
}

void ComboRank::LoadModels() {
	
	// モデル解放
	models_.fill(nullptr);
	
	// モデル読み込み
	models_[static_cast<size_t>(Rank::D)] = Model::CreateFromOBJ("rankD");
	models_[static_cast<size_t>(Rank::C)] = Model::CreateFromOBJ("rankC");
	models_[static_cast<size_t>(Rank::B)] = Model::CreateFromOBJ("rankB");
	models_[static_cast<size_t>(Rank::A)] = Model::CreateFromOBJ("rankA");
	models_[static_cast<size_t>(Rank::S)] = Model::CreateFromOBJ("rankS");
}

// ==========================================================================
// 加点と減点
// ==========================================================================

void ComboRank::AddHit(float value) {
	
	// コンボポイント加算
	comboPoint_ += value;
	comboPoint_ = std::clamp(comboPoint_, 0.0f, kMaxPoint);

	// 何か当てたので時間リセット
	decayTimer_ = 0.0f;

	UpdateRank();
}

void ComboRank::OnEnemyKilled(float bonus) {
	
	// 敵撃破ボーナス加算
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
	
	// コンボリセット
	comboPoint_ = 0.0f;
	decayTimer_ = 0.0f;
	rank_ = Rank::None;
}

// ==========================================================================
// ランク計算と更新
// ==========================================================================

void ComboRank::UpdateRank() {
	
	// ランク更新
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

// ==========================================================================
// 更新処理
// ==========================================================================

void ComboRank::Update(float deltaTime) {
	
	// 時間経過でコンボポイントを減衰させる
	decayTimer_ += deltaTime;

	// 一定時間経過後に減衰開始
	if (decayTimer_ > kStartDecayTime && comboPoint_ > 0.0f) {
		comboPoint_ -= kDecaySpeed * deltaTime;
		if (comboPoint_ < 0.0f) {
			comboPoint_ = 0.0f;
		}
		UpdateRank();
	}

	/*
	// --- ゲージの更新 ---
	*/

	// ポイントの計算
	float ratio = comboPoint_ / kMaxPoint;
	ratio = std::clamp(ratio, 0.0f, 1.0f);

	// ゲージのスケールを調整
	wtGauge_.scale_.x = kGaugeScale.x * ratio;
	wtGauge_.scale_.y = kGaugeScale.y;
	wtGauge_.scale_.z = kGaugeScale.z;

	// 左詰めにするための座標計算
	float modelHalfWidth = 3.0f;
	float shiftAmount = (1.0f - ratio) * modelHalfWidth * kGaugeScale.x;

	wtGauge_.translation_ = wtBar_.translation_;
	wtGauge_.translation_.x -= shiftAmount;

	// 行列更新 (ランクのえいご)
	wt_.matWorld_ = MakeAffineMatrix(wt_.scale_, wt_.rotation_, wt_.translation_);
	wt_.TransferMatrix();

	// 行列更新 (ゲージ枠)
	wtBar_.matWorld_ = MakeAffineMatrix(wtBar_.scale_, wtBar_.rotation_, wtBar_.translation_);
	wtBar_.TransferMatrix();

	// 行列更新 (ゲージ中身)
	wtGauge_.matWorld_ = MakeAffineMatrix(wtGauge_.scale_, wtGauge_.rotation_, wtGauge_.translation_);
	wtGauge_.TransferMatrix();
}

// ==========================================================================
// 描画処理
// ==========================================================================

void ComboRank::Draw() {
	
	if (!uiCamera_) {
		return;
	}

	// ランクなし
	if (rank_ == Rank::None) {
		return;
	}
	
	if (rank_ == Rank::None && comboPoint_ <= 0.0f) {
		return;
	}

	// ゲージ枠描画
	if (modelBar_) {
		modelBar_->Draw(wtBar_, *uiCamera_);
	}
	// ゲージ中身描画
	if (modelGauge_) {
		modelGauge_->Draw(wtGauge_, *uiCamera_);
	}

	// ランク文字描画
	if (rank_ != Rank::None) {
		size_t idx = static_cast<size_t>(rank_);
		if (idx < models_.size()) {
			Model* model = models_[idx];
			if (model) {
				model->Draw(wt_, *uiCamera_);
			}
		}
	}
}
