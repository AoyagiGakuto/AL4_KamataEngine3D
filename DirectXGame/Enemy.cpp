#include "Enemy.h"
#include "MyMath.h"
#include <cassert>

using namespace KamataEngine;

void Enemy::Initialize(Model* model, Camera* camera, const Vector3& position) {
	assert(model);
	model_ = model;
	camera_ = camera;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	velocity_ = {-kWalkSpeed, 0.0f, 0.0f};
	walkTimer_ = 0.0f; // 初期化
}

void Enemy::Update() {
	// 敵の歩行モーションのタイマーを更新
	walkTimer_ += 1.0f / 60.0f; // フレームレートに応じて調整
	// 歩行モーションの角度を計算
	// 回転アニメーション
	float param = std::sin(walkTimer_ * (std::numbers::pi_v<float> * 2.0f / kWalkMotionTime)) * (kWalkMotionAngelEnd - kWalkMotionAngelStart) + kWalkMotionAngelStart;

	float degree = kWalkMotionAngelStart + kWalkMotionAngelEnd * (param + 1.0f) / 2.0f; // 角度を計算

	// x軸周りの角度をラジアンに変換
	worldTransform_.rotation_.x = (degree);

	// X方向にゆっくり移動
	worldTransform_.translation_.x += velocity_.x;

	// 行列更新
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

void Enemy::Draw() {
	if (model_ && camera_) {
		model_->Draw(worldTransform_, *camera_);
	}
}

void Enemy::OnCollision(const Player* player) { (void)player; }

AABB Enemy::GetAABB() {
	AABB aabb;

	aabb.min = {
		worldTransform_.translation_.x - Enemy::kWidth / 2.0f,
		worldTransform_.translation_.y - Enemy::kHeight / 2.0f,
	    worldTransform_.translation_.z - Enemy::kWidth / 2.0f
	};

	aabb.max = {
	    worldTransform_.translation_.x + Enemy::kWidth / 2.0f, 
		worldTransform_.translation_.y + Enemy::kHeight / 2.0f,
	    worldTransform_.translation_.z + Enemy::kWidth / 2.0f
	};

	return aabb;
}