#include "Player.h"
using namespace KamataEngine;

void Player::Initialize(Model* model, uint32_t textureHandle, Camera* camera) {
	// NULLポインタチェック
	assert(model);
	// モデルの設定
	model_ = model;
	// テクスチャハンドルの設定
	textureHandle_ = textureHandle;
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
