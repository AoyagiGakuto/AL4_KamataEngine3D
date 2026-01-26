#include "Bullet.h"
#include <algorithm>

using namespace KamataEngine;

// ==========================================================================
// 初期化処理
// ==========================================================================

void Bullet::Initialize(Model* model, Camera* camera, const Vector3& pos, const Vector3& dir) {
	
	// モデルとカメラをセット
	model_ = model;
	camera_ = camera;
	
	// ワールド変換情報の初期化
	transform_.Initialize();
	transform_.translation_ = pos;
	transform_.scale_ = {0.2f, 0.2f, 0.2f};
	
	// 進行方向の正規化
	direction_ = dir;
	float len = std::sqrt(direction_.x * direction_.x + direction_.y * direction_.y + direction_.z * direction_.z);
	
	// ゼロ除算防止
	// 分母が0にならないように
	if (len > 0.0001f) {
		direction_.x /= len;
		direction_.y /= len;
		direction_.z /= len;
	}

	// 生存フラグと寿命の初期化
	alive_ = true;
	lifetime_ = 2.0f; // 2秒間
}

// ==========================================================================
// 更新処理
// ==========================================================================

void Bullet::Update() {

	// 死んでいたら更新しない
	if (!alive_) {
		return;
	}

	// 位置更新
	transform_.translation_.x += direction_.x * speed_;
	transform_.translation_.y += direction_.y * speed_;
	transform_.translation_.z += direction_.z * speed_;

	// 寿命更新
	lifetime_ -= 1.0f / 60.0f;
	
	// 寿命切れチェック
	if (lifetime_ <= 0.0f) {
		alive_ = false;
	}

	// 行列更新
	transform_.matWorld_ = MakeAffineMatrix(transform_.scale_, transform_.rotation_, transform_.translation_);
	transform_.TransferMatrix();
}

// ==========================================================================
// 描画処理
// ==========================================================================

void Bullet::Draw() {

	// 生存していなかったら描画しない
	if (!alive_ || !model_ || !camera_) {
		return;
	}

	// 描画
	model_->Draw(transform_, *camera_);
}

// ==========================================================================
// 当たり判定
// ==========================================================================

AABB Bullet::GetAABB() const {
	
	AABB aabb;
	
	// 弾の大きさ
	const float half = 0.2f * 0.5f;

	// 最小
	aabb.min = {transform_.translation_.x - half, transform_.translation_.y - half, transform_.translation_.z - half};
	
	// 最大
	aabb.max = {transform_.translation_.x + half, transform_.translation_.y + half, transform_.translation_.z + half};
	
	return aabb;
}
