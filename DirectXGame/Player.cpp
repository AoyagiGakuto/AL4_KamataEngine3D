#include "Player.h"
using namespace KamataEngine;

void Player::Initialize(Model* model, uint16_t extureHandle_, Camera* camera) {
	// NULLポインタチェック
	assert(model);
	// モデルの設定
	model_ = model;
	// テクスチャハンドルの設定
	textureHandle_ = extureHandle_;
	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// カメラの設定
	camera_ = camera;
}

void Player::Update() {
	// ワールドトランスフォームの更新
	worldTransform_.TransferMatrix();

}

void Player::Draw() {
	// モデルの描画
	model_->Draw(worldTransform_, *camera_, textureHandle_);
}
