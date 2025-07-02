#include "GameScene.h"
#include "CameraController.h"
#include "MyMath.h"
#include <numbers>

using namespace KamataEngine;

//========================================
// 初期化処理
//========================================

void GameScene::Initialize() {
	// モデルの読み込み
	modelCube_ = Model::CreateFromOBJ("block");
	modelSkyDome_ = Model::CreateFromOBJ("SkyDome", true);
	modelPlayer_ = Model::CreateFromOBJ("player");
	modelEnemy_ = Model::CreateFromOBJ("Ninja");
	modelDearthParticles_ = Model::CreateFromOBJ("block");

	//dearthParticles
	// モデルの作成
	model_ = Model::Create();
	mapChipField_ = new MapChipField();

	player_ = new Player();

	// マップチップデータの読み込み
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// カメラのインスタンス作成
	camera_ = new Camera();

	dearthParticles_ = new DearthParticles(modelDearthParticles_);
	
	// カメラの初期化
	camera_->Initialize();

	GenerateBlooks();

	// 座標をマップチップ番号で指定
	Vector3 playerPosition = mapChipField_->GetMapPositionTypeByIndex(25, 18);

	// プレイヤーの初期化
	player_->Initialize(modelPlayer_, camera_, playerPosition); 

	player_->SetMapChipField(mapChipField_);

	// enemyPosition.y -= MapChipField::kBlockHeight / 2.0f; // "kBlockHeight" を静的メンバーとしてアクセス

	for (int32_t i = 0; i < 1; ++i) {
		Enemy* newEnemy = new Enemy();
		Vector3 enemyPosition = mapChipField_->GetMapPositionTypeByIndex(30 + i, 18);
		enemyPosition.y -= MapChipField::kBlockHeight / 2.0f; // "kBlockHeight" を静的メンバーとしてアクセス
		newEnemy->Initialize(modelEnemy_, camera_, enemyPosition);
		newEnemy->SetMapChipField(mapChipField_);
		newEnemy->SetScale({0.4f, 0.4f, 0.4f});                          // Enemyの大きさを小さくする
		newEnemy->SetRotationY(std::numbers::pi_v<float> * 3.0f / 2.0f); // EnemyをPlayerとは逆方向に向ける
		enemies_.push_back(newEnemy);
	}

	// カメラコントロールの初期化
	cameraController_ = new CameraController();
	cameraController_->SetTarget(player_);
	CameraController::Rect cameraArea = {12.0f, 100 - 12.0f, 6.0f, 6.0f};
	cameraController_->SetMovableArea(cameraArea);
	cameraController_->Initialize();
	cameraController_->Reset();
}

void GameScene::GenerateBlooks() {
	// 要素数
	uint32_t kNumBlockVertical = mapChipField_->GetNumBlockVertical();
	uint32_t kNumBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	// 要素数を変更する
	worldTransformBlocks_.resize(kNumBlockVertical);

	for (uint32_t i = 0; i < kNumBlockVertical; ++i) {
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}

	// キューブの生成
	for (uint32_t i = 0; i < kNumBlockVertical; ++i) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapPositionTypeByIndex(j, i);
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

	player_->Update();

	dearthParticles_->Update();

	for (Enemy* enemy : enemies_) {
		enemy->Update();
	}

	// ここで衝突判定を呼び出す
	CheckAllCollisions();

	cameraController_->Update();

	if (Input::GetInstance()->PushKey(DIK_O)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
	// カメラの更新
	if (isDebugCameraActive_) {
		debugCamera_->Update();
		camera_->matView = debugCamera_->GetCamera().matView;
		camera_->matProjection = debugCamera_->GetCamera().matProjection;
	} else {
		camera_->matView = cameraController_->GetViewProjection().matView;
		camera_->matProjection = cameraController_->GetViewProjection().matProjection;
		camera_->TransferMatrix();
	}
}

void GameScene::CheckAllCollisions() {
#pragma region 自キャラと敵キャラの当たり判定
	AABB aabb1, aabb2;
	aabb1 = player_->GetAABB();
	for (Enemy* enemy : enemies_) {
		aabb2 = enemy->GetAABB();
		// AABBの交差判定
		if (aabb1.min.x < aabb2.max.x && aabb1.max.x > aabb2.min.x &&
			aabb1.min.y < aabb2.max.y && aabb1.max.y > aabb2.min.y &&
			aabb1.min.z < aabb2.max.z && aabb1.max.z > aabb2.min.z) {
			// 当たった場合の処理
			player_->OnCollision(enemy);
			enemy->OnCollision(player_);
		}
	}
#pragma endregion 自キャラと敵キャラの当たり判定
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

	player_->Draw();

	// パーティクルの描画を追加
	dearthParticles_->Draw();

	for (Enemy* enemy : enemies_) {
		enemy->Draw();
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
	delete modelCube_;
	delete modelSkyDome_;
	delete modelDearthParticles_;
	delete dearthParticles_;
	dearthParticles_ = nullptr;
	delete debugCamera_;
	delete cameraController_;
	delete mapChipField_;
	delete player_;
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();
}
