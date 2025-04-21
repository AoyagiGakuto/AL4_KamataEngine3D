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

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// カメラのインスタンス作成
	camera_ = new Camera();

	// カメラの初期化
	if (camera_) {
		camera_->Initialize();
	}

	// 要素数
	const uint32_t kNumBlockHorizontal = 20;

	// ブロック1個分の横幅
	const float kBlockWidth = 2.0f;

	// 要素数を変更する
	worldTransformBlocks_.resize(kNumBlockHorizontal);

	// キューブの生成
	for (uint32_t i = 0; i < kNumBlockHorizontal; ++i) {
		// ワールドトランスフォームのインスタンスを生成
		worldTransformBlocks_[i] = new WorldTransform();
		// ワールドトランスフォームの初期化
		worldTransformBlocks_[i]->Initialize();
		// ワールドトランスフォームの位置を設定
		worldTransformBlocks_[i]->translation_.x = kBlockWidth * i;
		// ワールドトランスフォームのスケールを設定
		worldTransformBlocks_[i]->translation_.y = 0.0f;
	}
}

//========================================
// 更新処理
//========================================

void GameScene::Update() {
	// 更新処理
	for (WorldTransform* worldTransformBlock : worldTransformBlocks_) {
		// アフィン変換行列の作成
		worldTransformBlock->matWorld_ = worldTransformBlock->matScale_ * worldTransformBlock->matRotation_ * worldTransformBlock->matTranslation_;
		// 定数バッファに転送する
		worldTransformBlock->TransferMatrix();
	}
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
	// model_->Draw(worldTransform_, *camera_, textureHandle_);

	for (WorldTransform* worldTransformBlock : worldTransformBlocks_) {
		// モデルの描画
		model_->Draw(*worldTransformBlock, *camera_, textureHandle_);
	}

	// スプライト描画後処理
	Model::PostDraw();
}

//========================================
// デストラクタ
//========================================

GameScene::~GameScene() {
	// モデルの解放
	delete model_;
	for (WorldTransform* worldTransformBlock : worldTransformBlocks_) {
		delete worldTransformBlock;
	}
	worldTransformBlocks_.clear();
}
