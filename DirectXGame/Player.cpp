#include "Player.h"
#include <numbers>

using namespace KamataEngine;
using namespace MathUtility;

void Player::Initialize(Model* model, Camera* camera, const Vector3& position) {
	// NULLポインタチェック
	assert(model);

	// モデルの設定
	modelPlayer_ = model;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;

	// カメラの設定
	camera_ = camera;
}

void Player::Update() {
	// ワールドトランスフォームの更新
	worldTransform_.TransferMatrix();
	// 移動入力
	if(Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {
		// 加速
		Vector3 acceleration = {};
		if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
			acceleration.x += kAcceleration;
			velocity_ += acceleration;
		}
	}

	// 移動
	worldTransform_.translation_ += velocity_;
	// 行列更新


}

void Player::Draw() {
	// モデルの描画
	modelPlayer_->Draw(worldTransform_, *camera_, textureHandle_);
}
