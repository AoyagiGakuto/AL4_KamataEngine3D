
#include "GameScene.h"
#include "CameraController.h"
#include "MyMath.h"
#include <numbers>
#include <cstdlib>
#include <ctime>

using namespace KamataEngine;

void GameScene::Initialize() {
	modelCube_ = Model::CreateFromOBJ("block");
	modelSkyDome_ = Model::CreateFromOBJ("SkyDome", true);
	modelPlayer_ = Model::CreateFromOBJ("player");
	modelEnemy_ = Model::CreateFromOBJ("Ninja");
	modelDeathParticle_ = Model::CreateFromOBJ("deathParticle");
	modelBullet_ = Model::CreateFromOBJ("bullet");
	modelSlowBall_ = Model::CreateFromOBJ("bullet");

	for (int i = 0; i < 10; ++i) {
		modelNumbers_[i] = Model::CreateFromOBJ(std::to_string(i));
	}

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
	score_ = 0;
	fade_ = new Fade();
	fade_->Initialize();
	phase_ = Phase::kFadeIn;
	fade_->Start(Fade::Status::FadeIn, 1.0f);
	// 毎回違うパターンで弾が降るように
	srand((unsigned int)time(NULL));
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

	cameraController_->Update();

	if (isDebugCameraActive_) {
		debugCamera_->Update();
		camera_->matView = debugCamera_->GetCamera().matView;
		camera_->matProjection = debugCamera_->GetCamera().matProjection;
	} else {
		camera_->matView = cameraController_->GetViewProjection().matView;
		camera_->matProjection = cameraController_->GetViewProjection().matProjection;
		camera_->TransferMatrix();
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

		// 回頭中でなく、死んでいない時だけ発射
		if (player_->GetTurnTimer() <= 0.0f && !player_->isDead_) {

			// プレイヤーの位置を弾発射位置に調整
			Vector3 spawnPos = player_->GetWorldTransform().translation_;
			spawnPos.y += 0.3f; // プレイヤーの中心に合わせる

			Vector3 dir; // 発射方向

			// ロックオン中か？
			if (player_->IsLockedOn() && player_->GetTargetEnemy()) {
				// ロックオン中敵の座標を取得
				Vector3 targetPos = player_->GetTargetEnemy()->GetWorldTransform().translation_;

				// 敵の中心を狙う
				targetPos.y += 0.4f; // 敵の高さの半分 (仮)

				dir = targetPos - spawnPos;
				dir = Normalize(dir);

			} else {
				// プレイヤーの向いている方向にまっすぐ
				float y = player_->GetWorldTransform().rotation_.y;
				dir = {std::sin(y), 0.0f, -std::cos(y)};
			}

			auto b = std::make_unique<Bullet>();
			b->Initialize(modelBullet_ ? modelBullet_ : modelCube_, camera_, spawnPos, dir);
			bullets_.push_back(std::move(b));
		}
	}

	// Yキーでスロー弾発射
	if (Input::GetInstance()->TriggerKey(DIK_Y)) {
		// プレイヤーが死んでたら撃てない
		if (!player_->IsDead()) {

			const int kNumRainBalls = 50;       // 降らせる弾の数
			const float kRainAreaWidth = 20.0f; // 左右10 (計20) の範囲
			const float kRainHeight = 20.0f;    // 上空20の高さから

			// カメラの中心X座標を取得 (弾を降らせる中心)
			float centerX = cameraController_->GetViewProjection().translation_.x;

			for (int i = 0; i < kNumRainBalls; ++i) {

				// 0.0f ～ 1.0fランダムな値
				float randomRatio = (float)(rand() % 1000) / 999.0f;
				// ランダム
				float randomX = (randomRatio - 0.5f) * kRainAreaWidth;

				Vector3 spawnPos;
				spawnPos.x = centerX + randomX; // カメラ中心 + ランダムなX
				spawnPos.y = kRainHeight;       // 固定の高さ
				spawnPos.z = 0.0f;              // Zは0ほかはなんかすんごいことになる

				Vector3 dir = {0.0f, -0.5f, 0.0f}; // ゆっくり真下に落ちる

				auto sb = std::make_unique<Bullet>();
				sb->Initialize(modelSlowBall_ ? modelSlowBall_ : modelCube_, camera_, spawnPos, dir);
				slowBalls_.push_back(std::move(sb));
			}
		}
	}

	if (Input::GetInstance()->TriggerKey(DIK_LSHIFT) || Input::GetInstance()->TriggerKey(DIK_RSHIFT)) {
		if (player_->IsLockedOn()) {
			// 既にロックオン中なら解除
			player_->LockOff();
		} else {
			// ロックオン開始
			Enemy* closestEnemy = nullptr;
			float minDistance = FLT_MAX; // 最小距離
			Vector3 playerPos = player_->GetWorldTransform().translation_;

			// 全ての敵をチェック
			for (Enemy* enemy : enemies_) {
				if (!enemy || enemy->IsDead()) {
					continue; // 死んでる敵は無視
				}

				Vector3 enemyPos = enemy->GetWorldTransform().translation_;

				// 距離の2乗で比較（平方根(sqrt)の計算を省略するため）
				float distanceSq = Length(enemyPos - playerPos); // Length関数を呼び出す

				if (distanceSq < minDistance) {
					minDistance = distanceSq;
					closestEnemy = enemy;
				}
			}

			// 一番近い敵が見つかったらロックオンとりあえず	
			if (closestEnemy) {
				player_->LockOn(closestEnemy);
			}
		}
	}

	// ロックオン対象が死んだらロックを解除
	if (player_->IsLockedOn()) {
		if (player_->GetTargetEnemy() == nullptr || player_->GetTargetEnemy()->IsDead()) {
			player_->LockOff();
		}
	}


	// 弾の更新と削除
	for (auto& b : bullets_) {
		b->Update();
	}

	// 削除
	bullets_.erase(std::remove_if(bullets_.begin(), bullets_.end(), [](const std::unique_ptr<Bullet>& b) { return !b->IsAlive(); }), bullets_.end());

	for (auto& sb : slowBalls_) {
		sb->Update();
	}

	slowBalls_.erase(std::remove_if(slowBalls_.begin(), slowBalls_.end(), [](const std::unique_ptr<Bullet>& sb) { return !sb->IsAlive(); }), slowBalls_.end());

	for (auto it = bullets_.begin(); it != bullets_.end();) {
		bool bulletRemoved = false;
		AABB aabbB = (*it)->GetAABB();
		for (auto enemyIt = enemies_.begin(); enemyIt != enemies_.end();) {
			AABB aabbE = (*enemyIt)->GetAABB();
			bool isHit =
			    (aabbB.min.x < aabbE.max.x && aabbB.max.x > aabbE.min.x) && (aabbB.min.y < aabbE.max.y && aabbB.max.y > aabbE.min.y) && (aabbB.min.z < aabbE.max.z && aabbB.max.z > aabbE.min.z);

			if (isHit) {

				// HP制に
				(*enemyIt)->TakeDamage(1); // 1ダメージ

				if ((*enemyIt)->IsDead()) { // 敵がHP0かチェック
					// 敵が死んだ時の処理
					score_++;
					hitEffects_.clear();
					auto hit = std::make_unique<HitEffect>();
					hit->Initialize(modelNumbers_, camera_, {0.0f, 0.0f, 0.0f}, score_);
					hitEffects_.push_back(std::move(hit));
					deathParticle_.Spawn((*enemyIt)->GetWorldTransform().translation_);

					delete *enemyIt;
					enemyIt = enemies_.erase(enemyIt); 

				} else {
					// 敵がまだ生きている
					++enemyIt; // 次の敵へ
				}

				(*it)->Kill(); // 弾は消える
				bulletRemoved = true;
				break; // この弾は処理したので次の弾へ

			} else {
				++enemyIt; // ヒットしなかったので次の敵へ
			}
		}
		if (bulletRemoved) {
			it = bullets_.erase(it);
		} else {
			++it;
		}
	}

	// スロー弾と敵の当たり判定
	for (auto it = slowBalls_.begin(); it != slowBalls_.end();) {
		bool ballRemoved = false;
		AABB aabbB = (*it)->GetAABB();
		for (auto enemyIt = enemies_.begin(); enemyIt != enemies_.end(); ++enemyIt) {
			AABB aabbE = (*enemyIt)->GetAABB();
			bool isHit =
			    (aabbB.min.x < aabbE.max.x && aabbB.max.x > aabbE.min.x) && (aabbB.min.y < aabbE.max.y && aabbB.max.y > aabbE.min.y) && (aabbB.min.z < aabbE.max.z && aabbB.max.z > aabbE.min.z);

			if (isHit) {
				// ダメージはなし
				(*enemyIt)->SlowDown(2.0f); // 2秒間スローにする

				(*it)->Kill(); // 球は消える
				ballRemoved = true;
				break; // この球は処理したので次の球へ
			}
		}
		if (ballRemoved) {
			it = slowBalls_.erase(it);
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

	// スロー弾とブロック・地面の当たり判定
	for (auto it = slowBalls_.begin(); it != slowBalls_.end();) {
		AABB aabbB = (*it)->GetAABB();
		MapChipField::IndexSet idx = mapChipField_->GetMapChipIndexSetByPosition((*it)->GetAABB().min);
		MapChipType type = mapChipField_->GetMapChipTypeByIndex(idx.xIndex, idx.yIndex);
		bool hitBlock = (type == MapChipType::kBlock);

		// 地面 (Y=0より下) またはブロックに当たったら消える
		if (hitBlock || aabbB.min.y < 0.0f) {
			(*it)->Kill();
			it = slowBalls_.erase(it);
		} else {
			++it;
		}
	}

	// カメラの座標を取得
	Vector3 cameraPos = cameraController_->GetViewProjection().translation_;

	// スコア表示の基準座標を計算
	Vector3 basePos = cameraPos;

	for (auto& hit : hitEffects_) {
		hit->Update(); // yOffset_ や Alpha の更新

		// 毎フレーム計算した「カメラの前方右上」の座標を渡して、エフェクトの位置を強制的に更新
		hit->UpdatePosition(basePos);
	}

	// 生存時間が切れたものを削除
	hitEffects_.erase(std::remove_if(hitEffects_.begin(), hitEffects_.end(), [](const std::unique_ptr<HitEffect>& h) { return !h->IsAlive(); }), hitEffects_.end());

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
			//player_->Die();

			// 死亡演出（パーティクル発生）
			//deathParticle_.Spawn(player_->GetWorldTransform().translation_);

			//break; // 死亡したらループ終了
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

	// スロー弾の描画
	for (const auto& sb : slowBalls_) {
		sb->Draw();
	}

	for (const auto& hit : hitEffects_) {
		hit->Draw();
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
	delete modelSlowBall_;
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

	for (Model* model : modelNumbers_) {
		delete model;
	}

	worldTransformBlocks_.clear();
}
