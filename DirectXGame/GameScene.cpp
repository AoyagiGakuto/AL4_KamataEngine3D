#include "GameScene.h"
#include "MyMath.h"

using namespace KamataEngine;

//========================================
// 初期化処理
//========================================

void GameScene::Initialize() {
	// モデルの読み込み
	modelBlock_ = Model::CreateFromOBJ("cube");

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
	const uint32_t kNumBlockVertical = 10;
	const uint32_t kNumBlockHorizontal = 20;
	// ブロック1個分の縦幅
	const float kBlockWidth = 2.0f;
	const float kBlockHeight = 2.0f;

	// 要素数を変更する
	worldTransformBlocks_.resize(kNumBlockHorizontal);

	for (uint32_t i = 0; i < kNumBlockVertical; ++i) {
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}
	
	// キューブの生成
	for (uint32_t i = 0; i < kNumBlockHorizontal; ++i) {
		for (uint32_t j = 0; j < kNumBlockVertical; ++j) {
			// ワールドトランスフォームのインスタンスを生成
			worldTransformBlocks_[i][j] = new WorldTransform();
			// ワールドトランスフォームの初期化
			worldTransformBlocks_[i][j]->Initialize();
			// ワールドトランスフォームの位置を設定
			worldTransformBlocks_[i][j]->translation_.x = kBlockWidth * j;
			// ワールドトランスフォームの位置を設定
			worldTransformBlocks_[i][j]->translation_.y = kBlockHeight * i;
		}
	}
}

//========================================
// 更新処理
//========================================

void GameScene::Update() {
	// 更新処理
	for (WorldTransform* worldTransformBlock : worldTransformBlocks_) {
		// アフィン変換行列の作成
		worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);
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
		model_->Draw(*worldTransformBlock, *camera_);
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
