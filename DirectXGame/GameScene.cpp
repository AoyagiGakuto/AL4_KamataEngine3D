#include "GameScene.h"

using namespace KamataEngine;

//========================================
// 初期化処理
//========================================

void GameScene::Initialize() {
	// 初期化処理

	// テクスチャの読み込み
	textureHandle_ = TextureManager::Load("white1x1.png");
	model_ = Model::Create();

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// カメラの初期化
	camera_->Initialize();
}

//========================================
// 更新処理
//========================================

void GameScene::Update() {
	// 更新処理
}

//========================================
// 描画処理
//========================================

void GameScene::Draw() {
	// 描画処理

	// DirectXCommonのインスタンスを取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// スプライト描画前処理
	Model::PreDraw(dxCommon->GetCommandList());

    model_->Draw(worldTransform_, *camera_, textureHandle_);

	// スプライト描画後処理
	Model::PostDraw();
}


