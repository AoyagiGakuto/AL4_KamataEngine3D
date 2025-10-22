
#include "GameScene.h"
#include "CameraController.h"
#include "MyMath.h"
#include <numbers>

using namespace KamataEngine;

void GameScene::Initialize() {
	modelCube_ = Model::CreateFromOBJ("block");
	modelSkyDome_ = Model::CreateFromOBJ("SkyDome", true);
	modelPlayer_ = Model::CreateFromOBJ("player");
	modelEnemy_ = Model::CreateFromOBJ("Ninja");
	modelDeathParticle_ = Model::CreateFromOBJ("deathParticle");
	modelBullet_ = Model::CreateFromOBJ("bullet");

	model_ = Model::Create();
	mapChipField_ = new MapChipField();
	player_ = new Player();

	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");

	worldTransform_.Initialize();

	camera_ = new Camera();
	camera_->Initialize();

	GenerateBlooks();

	// プレイヤー初期位置
	Vector3 playerPosition = mapChipField_->GetMapPositionTypeByIndex(25, 18);
	player_->Initialize(modelPlayer_, camera_, playerPosition);
	player_->SetMapChipField(mapChipField_);

	// 敵配置
	const int enemyCount = 2;
	for (int32_t i = 0; i < enemyCount; ++i) {
		Enemy* newEnemy = new Enemy();
		Vector3 enemyPosition = mapChipField_->GetMapPositionTypeByIndex(30 + i * 3, 18);
		newEnemy->Initialize(modelEnemy_, camera_, enemyPosition);
		newEnemy->SetMapChipField(mapChipField_);
		newEnemy->SetScale({0.4f, 0.4f, 0.4f});
		newEnemy->SetRotationY(std::numbers::pi_v<float> * 3.0f / 2.0f);

		if (i == 1) {
			// 2体目だけプレイヤーを追尾
			newEnemy->SetTarget(player_);
			newEnemy->SetHoming(true);
			// 速度パラメータ調整も可（速め）
			newEnemy->SetHomingParams(0.08f, 0.006f, 0.05f);
		} else {
			// 通常敵は追尾しない（false）
			newEnemy->SetHoming(false);
		}

		enemies_.push_back(newEnemy);
	}


	// カメラコントローラー
	cameraController_ = new CameraController();
	cameraController_->SetTarget(player_);
	CameraController::Rect cameraArea = {12.0f, 100 - 12.0f, 6.0f, 6.0f};
	cameraController_->SetMovableArea(cameraArea);
	cameraController_->Initialize();
	cameraController_->Reset();

	// DeathParticle 初期化
	deathParticle_.Initialize(modelDeathParticle_, camera_);

	particleCooldown_ = 0.0f;

	fade_ = new Fade();
	fade_->Initialize();
	phase_ = Phase::kFadeIn;
	fade_->Start(Fade::Status::FadeIn, 1.0f);
}

void GameScene::GenerateBlooks() {
	uint32_t kNumBlockVertical = mapChipField_->GetNumBlockVertical();
	uint32_t kNumBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	worldTransformBlocks_.resize(kNumBlockVertical);
	for (uint32_t i = 0; i < kNumBlockVertical; ++i) {
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}

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

void GameScene::Update() {
	if (particleCooldown_ > 0.0f) {
		particleCooldown_ -= 1.0f / 60.0f;
	}

	// マップブロック更新
	for (auto& line : worldTransformBlocks_) {
		for (auto& block : line) {
			if (!block)
				continue;
			block->matWorld_ = MakeAffineMatrix(block->scale_, block->rotation_, block->translation_);
			block->TransferMatrix();
		}
	}

	// 発射入力
	if (Input::GetInstance()->TriggerKey(DIK_H)) {

		// 回頭中は発射しない
		if (player_->GetTurnTimer() > 0.0f||player_->isDead_) {
			return;
		}

		// プレイヤーの位置を弾発射位置に調整
		Vector3 spawnPos = player_->GetWorldTransform().translation_;
		spawnPos.y += 0.3f; // プレイヤーの中心に合わせる
		float y = player_->GetWorldTransform().rotation_.y;
		Vector3 dir = {std::sin(y), 0.0f, -std::cos(y)};

		auto b = std::make_unique<Bullet>();
		b->Initialize(modelBullet_ ? modelBullet_ : modelCube_, camera_, spawnPos, dir);
		bullets_.push_back(std::move(b));
	}

	// 弾の更新と削除
	for (auto& b : bullets_) {
		b->Update();
	}

	// 削除
	bullets_.erase(std::remove_if(bullets_.begin(), bullets_.end(), [](const std::unique_ptr<Bullet>& b) { return !b->IsAlive(); }), bullets_.end());

	// --- 弾と敵の当たり判定 ---
	for (auto it = bullets_.begin(); it != bullets_.end();) {
		bool bulletRemoved = false;
		AABB aabbB = (*it)->GetAABB();
		for (auto enemyIt = enemies_.begin(); enemyIt != enemies_.end(); ++enemyIt) {
			AABB aabbE = (*enemyIt)->GetAABB();
			bool isHit =
			    (aabbB.min.x < aabbE.max.x && aabbB.max.x > aabbE.min.x) && (aabbB.min.y < aabbE.max.y && aabbB.max.y > aabbE.min.y) && (aabbB.min.z < aabbE.max.z && aabbB.max.z > aabbE.min.z);
			if (isHit) {
				// 敵削除と弾削除、演出（パーティクル）発生
				deathParticle_.Spawn((*enemyIt)->GetWorldTransform().translation_);
				delete *enemyIt;
				enemies_.erase(enemyIt);
				(*it)->Kill();
				bulletRemoved = true;
				break;
			}
		}
		if (bulletRemoved) {
			it = bullets_.erase(it);
		} else {
			++it;
		}
	}

	// ブロックとの当たり判定
	for (auto it = bullets_.begin(); it != bullets_.end();) {

		// 弾の位置からタイルを取得
		MapChipField::IndexSet idx = mapChipField_->GetMapChipIndexSetByPosition((*it)->GetAABB().min);

		MapChipType type = mapChipField_->GetMapChipTypeByIndex(idx.xIndex, idx.yIndex);

		bool hitBlock = (type == MapChipType::kBlock);
		if (hitBlock) {
			(*it)->Kill();
			it = bullets_.erase(it); // 消し
		} else {
			++it; // 残す
		}
	}

	// プレイヤーは死亡後止まるけど、敵は常に動く
	if (!player_->IsDead()) {
		player_->Update();
		CheckAllCollisions(); // 衝突判定は生存中だけ
	}

	// 敵は死亡後も動く
	for (Enemy* enemy : enemies_) {
		enemy->Update();
	}

	// 死亡パーティクル演出
	deathParticle_.Update();


	switch (phase_) {
	case Phase::kFadeIn:
		fade_->Update();
		if (fade_->IsFinished()) {
			phase_ = Phase::kPlay;
		}
		break;

	case Phase::kPlay:
		// プレイヤー死亡 & パーティクル終了 → フェードアウト開始（出るとき）
		if (player_->IsDead() && deathParticle_.IsFinished()) {
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, 1.0f);
		}
		break;

	case Phase::kFadeOut:
		fade_->Update();
		if (fade_->IsFinished()) {
			finished_ = true;
		}
		break;
	}

	cameraController_->Update();

	if (Input::GetInstance()->PushKey(DIK_O)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}

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
	if (player_->IsDead())
		return; // すでに死亡なら判定しない

	AABB aabb1 = player_->GetAABB();

	for (Enemy* enemy : enemies_) {
		AABB aabb2 = enemy->GetAABB();

		bool isHit = (aabb1.min.x < aabb2.max.x && aabb1.max.x > aabb2.min.x) && (aabb1.min.y < aabb2.max.y && aabb1.max.y > aabb2.min.y) && (aabb1.min.z < aabb2.max.z && aabb1.max.z > aabb2.min.z);

		if (isHit) {
			// プレイヤー死亡！
			player_->Die();

			// 死亡演出（パーティクル発生）
			deathParticle_.Spawn(player_->GetWorldTransform().translation_);

			break; // 死亡したらループ終了
		}
	}
}

void GameScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	Model::PreDraw(dxCommon->GetCommandList());

	for (auto& line : worldTransformBlocks_) {
		for (auto& block : line) {
			if (!block)
				continue;
			modelCube_->Draw(*block, *camera_);
		}
	}

	modelSkyDome_->Draw(worldTransform_, *camera_);
	player_->Draw();
	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}

	// 弾の描画
	for (const auto& b : bullets_) {
		b->Draw();
	}

	// DeathParticle描画
	deathParticle_.Draw();

	Model::PostDraw();

	if (fade_)
		fade_->Draw();
}

GameScene::~GameScene() {
	delete model_;
	delete modelCube_;
	delete modelSkyDome_;
	delete debugCamera_;
	delete cameraController_;
	delete mapChipField_;
	delete player_;
	delete fade_;
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	for (auto& line : worldTransformBlocks_) {
		for (auto& block : line) {
			delete block;
		}
	}
	worldTransformBlocks_.clear();
}
