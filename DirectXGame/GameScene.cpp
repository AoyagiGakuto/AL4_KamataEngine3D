#include "GameScene.h"

using namespace KamataEngine;

//========================================
// 初期化処理
//========================================

void GameScene::Initialize() {
	// テクスチャの読み込み
	textureHandle_ = TextureManager::Load("sample.png");

	// モデルの作成
	model_ = Model::Create();
	player_ = new Player();

	// プレイヤーの初期化
	player_->Initialize(model_, textureHandle_, camera_);

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// カメラのインスタンス作成
	camera_ = new Camera();

	// カメラの初期化
	if (camera_) {
		camera_->Initialize();
	}
}

//========================================
// 更新処理
//========================================

void GameScene::Update() {
	// 更新処理
	player_->Update();
}

//========================================
// 描画処理
//========================================

void GameScene::Draw() {
	// DirectXCommonのインスタンスを取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// スプライト描画前処理
	Model::PreDraw(dxCommon->GetCommandList());

	// モデルの描画
	model_->Draw(worldTransform_, *camera_, textureHandle_);

	// プレイヤーの描画
	player_->Draw();

	// スプライト描画後処理
	Model::PostDraw();
}

//========================================
// デストラクタ
//========================================

GameScene::~GameScene() {
	// モデルの解放
	delete model_;
	delete player_;
}
