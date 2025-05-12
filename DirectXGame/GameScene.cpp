#include "GameScene.h"
#include "MyMath.h"

using namespace KamataEngine;

//========================================
// 初期化処理
//========================================

void GameScene::Initialize() {
	// モデルの読み込み
	modelCube_ = Model::CreateFromOBJ("block");
	modelSkyDome_ = Model::CreateFromOBJ("SkyDome", true);

	// モデルの作成
	model_ = Model::Create();
	mapChipField_ = new MapChipField();

	// マップチップデータの読み込み
	mapChipField_->LoadMapChipCsv("Resources/mapchip.csv");

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// カメラのインスタンス作成
	camera_ = new Camera();

	// カメラの初期化
	if (camera_) {
		camera_->Initialize();
	}
	GenerateBlooks() {
		// 要素数
		uint32_t kNumBlockVertical = mapChipField_->GetNumBlockVirtical();
		uint32_t kNumBlockHorizontal = mapChipField_->GetNumBlockHorizontal();
		// ブロック1個分の縦幅
		const float kBlockWidth = 2.0f;
		const float kBlockHeight = 2.0f;

		// 要素数を変更する
		worldTransformBlocks_.resize(kNumBlockVertical);

		for (uint32_t i = 0; i < kNumBlockVertical; ++i) {
			worldTransformBlocks_[i].resize(kNumBlockHorizontal);
		}

		// キューブの生成
		for (uint32_t i = 0; i < kNumBlockVertical; ++i) {
			for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
				// 市松模様条件：i + j が偶数のときだけ表示
				if (mapChipField_->GetMapChipTypeByIndex(j,i)==MapChipType::kBlock) {
					WorldTransform* worldTransformBlock = new WorldTransform();
					worldTransformBlock->Initialize();
					worldTransformBlocks_[i][j] = WorldTransform;
					worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapPositionTypeByIndex(j, i);
				}
			}
		}
	}
}

//========================================
// 更新処理
//========================================

void GameScene::Update() {
	// 更新処理
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock) {
				continue;
			}
			// アフィン変換行列の作成
			worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);
			// 定数バッファに転送する
			worldTransformBlock->TransferMatrix();
		}
	}

#ifdef DEBUG
	if (Input::GetInstance()->TriggerKeyPush(DIK_O) { isDebugCameraActive_ = !isDebugCameraActive_; })
		// カメラの更新
		if (isDebugCameraActive_) {
			debugCamera_->Update();
			camera_->matView_ = debugCamera_->GetCamera().matView;
			camera_->matProjection_ = debugCamera_->GetCamera().matProjection;
		} else {
			camera_->UpdateMatrix();
		}
#endif
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
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock) {
				continue;
			}
			modelCube_->Draw(*worldTransformBlock, *camera_);
		}
	}

	// スカイドームの描画
	modelSkyDome_->Draw(worldTransform_, *camera_);

	// スプライト描画後処理
	Model::PostDraw();
}

//========================================
// デストラクタ
//========================================

GameScene::~GameScene() {
	// モデルの解放
	delete model_;
	delete modelCube_;
	delete modelSkyDome_;
	delete debugCamera_;

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();
}
