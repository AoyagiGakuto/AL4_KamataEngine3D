#include "GameScene.h"
#include "CameraController.h"
#include "MyMath.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <numbers>

using namespace KamataEngine;
using namespace MathUtility;

// ==========================================================================
// 初期化処理
// ==========================================================================

void GameScene::Initialize() {
	// モデル生成
	modelCube_ = Model::CreateFromOBJ("block");
	modelSkyDome_ = Model::CreateFromOBJ("SkyDome", true);
	modelPlayer_ = Model::CreateFromOBJ("player");
	modelEnemy_ = Model::CreateFromOBJ("Ninja");
	modelDeathParticle_ = Model::CreateFromOBJ("deathParticle");
	modelBullet_ = Model::CreateFromOBJ("bullet");
	modelSlowBall_ = Model::CreateFromOBJ("bullet");
	modelZangeki_ = Model::CreateFromOBJ("zangeki");
	modelHp_ = Model::CreateFromOBJ("hp");
	modelHpBar_ = Model::CreateFromOBJ("hpbar");

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

	uiCamera_ = new Camera();
	uiCamera_->Initialize();

	Vector3 uiPos = {0.0f, 0.0f, -10.0f};
	Vector3 uiRot = {0.0f, 0.0f, 0.0f};
	Vector3 uiScale = {1.0f, 1.0f, 1.0f};

	// カメラのワールド行列を作成
	Matrix4x4 matWorld = MakeAffineMatrix(uiScale, uiRot, uiPos);

	// uiカメラ設定
	uiCamera_->matView = Inverse(matWorld);
	uiCamera_->TransferMatrix();

	// HPバー初期化
	worldTransformHudHpBar_.Initialize();
	worldTransformHudHp_.Initialize();

	Vector3 hudPos = {-3.5f, 3.8f, 0.0f};
	comboRank_.Initialize(uiCamera_, {1.0f, 3.5f, 0.0f});

	worldTransformHudHpBar_.translation_ = hudPos;
	worldTransformHudHp_.translation_ = hudPos;

	Vector3 hudScale = {1.2f, 0.2f, 0.1f};
	worldTransformHudHpBar_.scale_ = hudScale;
	worldTransformHudHp_.scale_ = hudScale;

	// ブロック生成
	GenerateBlooks();

	// プレイヤー初期位置
	Vector3 playerPosition = mapChipField_->GetMapPositionTypeByIndex(25, 18);
	player_->Initialize(modelPlayer_, camera_, playerPosition);
	player_->SetMapChipField(mapChipField_);

	// 敵配置
	const int enemyCount = 3; // 3体（ふつう敵、ついてくる敵、飛んで回復する敵）

	for (int32_t i = 0; i < enemyCount; ++i) {
		Enemy* newEnemy = new Enemy();

		// 敵の基本位置(仮)
		Vector3 enemyPosition = mapChipField_->GetMapPositionTypeByIndex(30 + i * 3, 18);

		// 全員プレイヤーをターゲットとしてセットしておく
		newEnemy->SetTarget(player_);

		// タイプを変える
		if (i == 0) {
			// 通常タイプ
			newEnemy->Initialize(modelEnemy_, modelHpBar_, modelHp_, camera_, enemyPosition, Enemy::Type::kNormal);
			newEnemy->SetScale({0.4f, 0.4f, 0.4f});

		} else if (i == 1) {
			// 追尾タイプ
			newEnemy->Initialize(modelEnemy_, modelHpBar_, modelHp_, camera_, enemyPosition, Enemy::Type::kHoming);
			newEnemy->SetScale({0.4f, 0.4f, 0.4f});

		} else {
			// 飛行支援タイプ
			enemyPosition.y += 5.0f;
			newEnemy->Initialize(modelEnemy_, modelHpBar_, modelHp_, camera_, enemyPosition, Enemy::Type::kFlyingSupport);
			newEnemy->SetScale({0.3f, 0.3f, 0.3f}); // 少し小さめ
		}

		newEnemy->SetMapChipField(mapChipField_);
		// 回転の初期値
		newEnemy->SetRotationY(std::numbers::pi_v<float> * 3.0f / 2.0f);

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
	fade_->Start(Fade::Status::FadeIn, 1.0f);
	// 毎回違うパターンで弾が降るように
	srand((unsigned int)time(NULL));
}

// ==========================================================================
// ブロック生成
// ==========================================================================

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

// ==========================================================================
// 更新処理
// ==========================================================================

void GameScene::Update() {
	const float dt = 1.0f / 60.0f;

	if (particleCooldown_ > 0.0f) {
		particleCooldown_ -= dt;
	}

	cameraController_->Update();

	// カメラ行列の更新
	if (isDebugCameraActive_) {
		debugCamera_->Update();
		camera_->matView = debugCamera_->GetCamera().matView;
		camera_->matProjection = debugCamera_->GetCamera().matProjection;
	} else {
		camera_->matView = cameraController_->GetViewProjection().matView;
		camera_->matProjection = cameraController_->GetViewProjection().matProjection;
		camera_->TransferMatrix();
	}

	// いろいろ更新処理
	UpdateSpecialMove(dt);
	UpdateMapBlocks();
	UpdatePlayerAction();

	// プレイヤーの移動更新
	if (!player_->IsDead()) {
		if (specialState_ == SpecialState::None) {
			player_->Update();
			CheckAllCollisions();
		}
	}

	UpdateEnemies();
	UpdateProjectiles();
	CheckCollisions();

	// エフェクト更新
	deathParticle_.Update();

	// 斬撃エフェクトの更新
	for (auto& vfx : SlashEffects_) {
		vfx->Update();
	}

	SlashEffects_.erase(std::remove_if(SlashEffects_.begin(), SlashEffects_.end(), [](const std::unique_ptr<SlashEffect>& v) { return !v->IsAlive(); }), SlashEffects_.end());

	UpdateHud();

	UpdateSceneFlow();
}

// ==========================================================================
// マップブロックの更新
// ==========================================================================

void GameScene::UpdateMapBlocks() {
	for (auto& line : worldTransformBlocks_) {
		for (auto& block : line) {
			if (!block)
				continue;
			block->matWorld_ = MakeAffineMatrix(block->scale_, block->rotation_, block->translation_);
			block->TransferMatrix();
		}
	}
}

// ==========================================================================
// 入力処理の更新
// ==========================================================================

void GameScene::UpdatePlayerAction() {

	/*
	// --- 通常弾発射 ---
	*/

	if (Input::GetInstance()->TriggerKey(DIK_H)) {
		if (player_->GetTurnTimer() <= 0.0f && !player_->isDead_) {
			Vector3 spawnPos = player_->GetWorldTransform().translation_;
			spawnPos.y += 0.3f;
			Vector3 dir;

			if (player_->IsLockedOn() && player_->GetTargetEnemy()) {
				Vector3 targetPos = player_->GetTargetEnemy()->GetWorldTransform().translation_;
				targetPos.y += 0.4f;
				Vector3 diff = targetPos - spawnPos;

				dir = Normalize(diff);
			} else {
				float y = player_->GetWorldTransform().rotation_.y;
				dir = {std::sin(y), 0.0f, -std::cos(y)};
			}

			auto b = std::make_unique<Bullet>();
			b->Initialize(modelBullet_ ? modelBullet_ : modelCube_, camera_, spawnPos, dir);
			bullets_.push_back(std::move(b));
		}
	}

	/*
	// ---スロー弾発射---
	*/

	if (Input::GetInstance()->TriggerKey(DIK_Y)) {
		if (!player_->IsDead() && player_->IsLockedOn()) {
			Enemy* targetEnemy = player_->GetTargetEnemy();
			if (targetEnemy && !targetEnemy->IsDead()) {
				const int kNumRainBalls = 10;
				const float kRainAreaWidth = 5.0f;
				const float kRainHeight = 10.0f;
				float centerX = targetEnemy->GetWorldTransform().translation_.x;

				for (int i = 0; i < kNumRainBalls; ++i) {
					float randomRatio = (float)(rand() % 1000) / 999.0f;
					float randomX = (randomRatio - 0.5f) * kRainAreaWidth;
					Vector3 spawnPos = {centerX + randomX, kRainHeight, 0.0f};
					Vector3 dir = {0.0f, -0.2f, 0.0f};

					auto sb = std::make_unique<Bullet>();
					sb->Initialize(modelSlowBall_ ? modelSlowBall_ : modelCube_, camera_, spawnPos, dir);
					slowBalls_.push_back(std::move(sb));
				}
			}
		}
	}

	/*
	// ---ロックオン制御---
	*/

	if (Input::GetInstance()->PushKey(DIK_LSHIFT) || Input::GetInstance()->PushKey(DIK_RSHIFT)) {
		bool needsNewTarget = true;
		if (player_->IsLockedOn()) {
			Enemy* currentTarget = player_->GetTargetEnemy();
			if (currentTarget && !currentTarget->IsDead()) {
				needsNewTarget = false;
			}
		}

		if (needsNewTarget) {
			Enemy* closestEnemy = nullptr;
			float minDistance = FLT_MAX;
			Vector3 playerPos = player_->GetWorldTransform().translation_;

			for (Enemy* enemy : enemies_) {
				if (!enemy || enemy->IsDead())
					continue;
				float distanceSq = Length(enemy->GetWorldTransform().translation_ - playerPos);
				if (distanceSq < minDistance) {
					minDistance = distanceSq;
					closestEnemy = enemy;
				}
			}
			player_->LockOn(closestEnemy);
		}

		// 切り替え
		if (Input::GetInstance()->TriggerKey(DIK_L) && player_->IsLockedOn()) {
			Enemy* currentTarget = player_->GetTargetEnemy();
			auto it = std::find(enemies_.begin(), enemies_.end(), currentTarget);
			if (it != enemies_.end()) {
				size_t checkCount = 0;
				size_t enemyMax = enemies_.size();
				while (checkCount < enemyMax) {
					it++;
					if (it == enemies_.end())
						it = enemies_.begin();
					if (!(*it)->IsDead()) {
						player_->LockOn(*it);
						break;
					}
					checkCount++;
				}
			}
		}
	} else {
		player_->LockOff();
	}

	/*
	// ---近接攻撃---
	*/

	if (Input::GetInstance()->TriggerKey(DIK_K)) {
		if (player_->IsLockedOn()) {
			Enemy* target = player_->GetTargetEnemy();
			if (target && !target->IsDead()) {
				const float kMeleeRange = 3.5f;
				float distance = Length(target->GetWorldTransform().translation_ - player_->GetWorldTransform().translation_);

				if (distance <= kMeleeRange) {
					target->TakeDamage(1);
					target->ApplyHitStop(0.1f);
					comboRank_.AddHit(8.0f);

					// エフェクト生成
					const float kEffectSpread = 1.0f;
					Vector3 enemyPos = target->GetWorldTransform().translation_;
					enemyPos.y += 0.1f;
					auto vfx = std::make_unique<SlashEffect>();
					float randX = ((float)(rand() % 1000) / 999.0f - 0.5f) * kEffectSpread;
					float randY = ((float)(rand() % 1000) / 999.0f - 0.5f) * kEffectSpread;
					vfx->Initialize(modelZangeki_, camera_, enemyPos + Vector3{randX, randY, 0.0f});
					vfx->SetRotation(player_->GetWorldTransform().rotation_.y);
					SlashEffects_.push_back(std::move(vfx));

					// 撃破処理
					if (target->IsDead()) {
						score_++;
						comboRank_.OnEnemyKilled(12.0f);
						deathParticle_.Spawn(target->GetWorldTransform().translation_);

						enemies_.remove(target);
						delete target;
						player_->LockOff();
					}
				}
			}
		}
	}

	/*
	// ---チャージ攻撃---
	*/

	if (player_->IsChargeAttackReady()) {
		player_->ConsumeChargeAttack();
		Enemy* target = player_->GetTargetEnemy();
		if (target && !target->IsDead()) {
			comboRank_.AddHit(15.0f);
			for (int i = 0; i < 5; ++i) {
				target->TakeDamage(1);
			}

			// エフェクト生成
			const int kNumSlashes = 20;
			const float kEffectSpread = 1.5f;
			Vector3 enemyPos = target->GetWorldTransform().translation_;
			enemyPos.y += 0.5f;

			for (int i = 0; i < kNumSlashes; ++i) {
				auto vfx = std::make_unique<SlashEffect>();
				float randX = ((float)(rand() % 1000) / 999.0f - 0.5f) * kEffectSpread;
				float randY = ((float)(rand() % 1000) / 999.0f - 0.5f) * kEffectSpread;
				vfx->Initialize(modelZangeki_, camera_, enemyPos + Vector3{randX, randY, 0.0f});
				vfx->SetRandomRotation();
				SlashEffects_.push_back(std::move(vfx));
			}

			// 撃破処理
			if (target->IsDead()) {
				score_++;
				comboRank_.OnEnemyKilled(12.0f);
				deathParticle_.Spawn(target->GetWorldTransform().translation_);

				enemies_.remove(target);
				delete target;
				player_->LockOff();
			}
		}
	}
}

// ==========================================================================
// 弾の更新処理
// ==========================================================================

void GameScene::UpdateProjectiles() {
	// 通常弾
	for (auto& b : bullets_) {
		b->Update();
	}
	bullets_.erase(std::remove_if(bullets_.begin(), bullets_.end(), [](const std::unique_ptr<Bullet>& b) { return !b->IsAlive(); }), bullets_.end());

	// スロー弾
	for (auto& sb : slowBalls_) {
		sb->Update();
	}
	slowBalls_.erase(std::remove_if(slowBalls_.begin(), slowBalls_.end(), [](const std::unique_ptr<Bullet>& sb) { return !sb->IsAlive(); }), slowBalls_.end());
}

// ==========================================================================
// 敵の更新処理
// ==========================================================================

void GameScene::UpdateEnemies() {
	for (Enemy* enemy : enemies_) {
		if (!enemy) {
			continue;
		}

		if (specialState_ == SpecialState::Dash) {
			continue;
		}

		enemy->Update();
	}

	// 回復行動
	for (Enemy* enemy : enemies_) {
		if (enemy->GetType() == Enemy::Type::kFlyingSupport) {
			enemy->HealNearbyEnemies(enemies_);
		}
	}
}

// ==========================================================================
// 当たり判定処理
// ==========================================================================

void GameScene::CheckCollisions() {
	
	/*
	// --通常弾と敵---
	*/

	for (auto it = bullets_.begin(); it != bullets_.end();) {
		bool bulletRemoved = false;
		AABB aabbB = (*it)->GetAABB();

		// 敵との判定
		auto enemyIt = enemies_.begin();
		while (enemyIt != enemies_.end()) {
			Enemy* enemy = *enemyIt;
			AABB aabbE = enemy->GetAABB();
			bool isHit =
			    (aabbB.min.x < aabbE.max.x && aabbB.max.x > aabbE.min.x) && (aabbB.min.y < aabbE.max.y && aabbB.max.y > aabbE.min.y) && (aabbB.min.z < aabbE.max.z && aabbB.max.z > aabbE.min.z);

			if (isHit) {
				enemy->TakeDamage(1);
				comboRank_.AddHit(5.0f);

				if (enemy->IsDead()) {
					score_++;
					comboRank_.OnEnemyKilled(10.0f);
					deathParticle_.Spawn(enemy->GetWorldTransform().translation_);

					delete enemy;
					enemyIt = enemies_.erase(enemyIt);
				} else {
					++enemyIt;
				}

				(*it)->Kill();
				bulletRemoved = true;
				break;
			} else {
				++enemyIt;
			}
		}

		if (bulletRemoved) {
			it = bullets_.erase(it);
			continue;
		}

		// 壁との判定 (残っていたら)
		MapChipField::IndexSet idx = mapChipField_->GetMapChipIndexSetByPosition((*it)->GetAABB().min);
		MapChipType type = mapChipField_->GetMapChipTypeByIndex(idx.xIndex, idx.yIndex);
		if (type == MapChipType::kBlock) {
			(*it)->Kill();
			it = bullets_.erase(it);
		} else {
			++it;
		}
	}

	/*
	// ---スロー弾と敵---
	*/

	for (auto it = slowBalls_.begin(); it != slowBalls_.end();) {
		bool ballRemoved = false;
		AABB aabbB = (*it)->GetAABB();

		for (Enemy* enemy : enemies_) {
			AABB aabbE = enemy->GetAABB();
			bool isHit =
			    (aabbB.min.x < aabbE.max.x && aabbB.max.x > aabbE.min.x) && (aabbB.min.y < aabbE.max.y && aabbB.max.y > aabbE.min.y) && (aabbB.min.z < aabbE.max.z && aabbB.max.z > aabbE.min.z);

			if (isHit) {
				enemy->SlowDown(2.0f);
				(*it)->Kill();
				ballRemoved = true;
				break;
			}
		}

		if (ballRemoved) {
			it = slowBalls_.erase(it);
			continue;
		}

		// 壁地面判定
		MapChipField::IndexSet idx = mapChipField_->GetMapChipIndexSetByPosition((*it)->GetAABB().min);
		MapChipType type = mapChipField_->GetMapChipTypeByIndex(idx.xIndex, idx.yIndex);
		if (type == MapChipType::kBlock || aabbB.min.y < 0.0f) {
			(*it)->Kill();
			it = slowBalls_.erase(it);
		} else {
			++it;
		}
	}
}

// ==========================================================================
// UI更新
// ==========================================================================

void GameScene::UpdateHud() {
	float hpRatio = player_->GetHp() / player_->GetMaxHp();
	hpRatio = std::clamp(hpRatio, 0.0f, 1.0f);

	float baseScaleX = 1.2f;
	float baseScaleY = 0.2f;

	worldTransformHudHp_.scale_.x = baseScaleX * hpRatio;
	worldTransformHudHp_.scale_.y = baseScaleY;
	worldTransformHudHp_.scale_.z = 0.1f;

	float modelHalfWidth = 3.0f;
	float shiftAmount = (1.0f - hpRatio) * modelHalfWidth * baseScaleX;

	worldTransformHudHp_.translation_ = worldTransformHudHpBar_.translation_;
	worldTransformHudHp_.translation_.x -= shiftAmount;

	worldTransformHudHpBar_.matWorld_ = MakeAffineMatrix(worldTransformHudHpBar_.scale_, worldTransformHudHpBar_.rotation_, worldTransformHudHpBar_.translation_);
	worldTransformHudHpBar_.TransferMatrix();

	worldTransformHudHp_.matWorld_ = MakeAffineMatrix(worldTransformHudHp_.scale_, worldTransformHudHp_.rotation_, worldTransformHudHp_.translation_);
	worldTransformHudHp_.TransferMatrix();

	comboRank_.Update(1.0f / 60.0f);
}

// ==========================================================================
// シーン遷移管理
// ==========================================================================

void GameScene::UpdateSceneFlow() {
	switch (phase_) {
	case ScenePhase::FadeIn:
		fade_->Update();
		if (fade_->IsFinished()) {
			phase_ = ScenePhase::Play;
		}
		break;

	case ScenePhase::Play:
		if (player_->IsDead() && deathParticle_.IsFinished()) {
			phase_ = ScenePhase::FadeOut;
			fade_->Start(Fade::Status::FadeOut, 1.0f);
		}
		break;

	case ScenePhase::FadeOut:
		fade_->Update();
		if (fade_->IsFinished()) {
			finished_ = true;
		}
		break;
	}
}

// ==========================================================================
// プレイヤーと敵の衝突判定
// ==========================================================================

void GameScene::CheckAllCollisions() {
	if (player_->IsDead()) {
		return;
	}

	AABB aabb1 = player_->GetAABB();

	for (Enemy* enemy : enemies_) {
		if (enemy->IsDead()) {
			continue;
		}

		AABB aabb2 = enemy->GetAABB();

		// 当たり判定
		bool isHit = (aabb1.min.x < aabb2.max.x && aabb1.max.x > aabb2.min.x) && (aabb1.min.y < aabb2.max.y && aabb1.max.y > aabb2.min.y) && (aabb1.min.z < aabb2.max.z && aabb1.max.z > aabb2.min.z);

		if (isHit) {
			// ダメージ処理
			player_->TakeDamage(1);

			// コンボランクを減らす
			comboRank_.OnPlayerDamaged();

			// ノックバック方向の計算
			Vector3 pPos = player_->GetWorldTransform().translation_;
			Vector3 ePos = enemy->GetWorldTransform().translation_;

			Vector3 dir = pPos - ePos;
			dir = Normalize(dir);

			// お互いに弾き飛ばす
			player_->Knockback(dir);       // プレイヤーは敵と逆方向へ
			enemy->Knockback(dir * -1.0f); // 敵はプレイヤーと逆方向へ

			// 死亡チェック
			if (player_->IsDead()) {
				deathParticle_.Spawn(player_->GetWorldTransform().translation_);
				comboRank_.Reset();
			}

			break;
		}
	}
}

// ==========================================================================
// 特殊攻撃の更新
// ==========================================================================

void GameScene::UpdateSpecialMove(float deltaTime) {
	// R押した瞬間に発動開始
	if (specialState_ == SpecialState::None) {
		if (!player_->IsDead() && Input::GetInstance()->TriggerKey(DIK_R)) {
			specialState_ = SpecialState::Charge;
			specialTimer_ = 3.0f; // 3秒チャージ
			specialFinalSlashesSpawned_ = false;
		}
	}

	switch (specialState_) {
	case SpecialState::None:
		// 何もしない
		break;

	case SpecialState::Charge:
		specialTimer_ -= deltaTime;
		if (specialTimer_ <= 0.0f) {
			// チャージ終了
			specialState_ = SpecialState::Dash;
			specialTimer_ = 0.7f; // ダッシュ全体の長さ（0.7秒くらい）
			specialHitInterval_ = 0.0f;
		}
		break;

	case SpecialState::Dash:
		// ワープと斬撃処理
		PerformSpecialDash(deltaTime);
		break;

	case SpecialState::Finish:
		specialTimer_ -= deltaTime;

		if (!specialFinalSlashesSpawned_) {
			specialFinalSlashesSpawned_ = true;

			for (auto it = enemies_.begin(); it != enemies_.end();) {
				Enemy* e = *it;
				if (!e || e->IsDead()) {
					++it;
					continue;
				}

				// 敵の位置
				Vector3 enemyPos = e->GetWorldTransform().translation_;
				enemyPos.y += 0.5f;

				// ここでダメージ
				e->TakeDamage(9999);// (調整必)

				if (e->IsDead()) {
					score_++;
					deathParticle_.Spawn(enemyPos);

					delete e;
					it = enemies_.erase(it);
				} else {
					++it;
				}
			}

			// 演出
			const int kNumSlashes = 200;

			uint32_t width = mapChipField_->GetNumBlockHorizontal();
			uint32_t height = mapChipField_->GetNumBlockVertical();

			for (int i = 0; i < kNumSlashes; ++i) {
				uint32_t ix = rand() % width;
				uint32_t iy = rand() % height;

				Vector3 pos = mapChipField_->GetMapPositionTypeByIndex(ix, iy);
				pos.y += 0.5f;

				auto vfx = std::make_unique<SlashEffect>();
				vfx->Initialize(modelZangeki_, camera_, pos);
				vfx->SetRandomRotation();
				SlashEffects_.push_back(std::move(vfx));
			}
		}

		if (specialTimer_ <= 0.0f) {
			specialState_ = SpecialState::None;
		}

		break;
	}
}

// ==========================================================================
// 必殺技の時のダッシュの実装
// ==========================================================================

void GameScene::PerformSpecialDash(float deltaTime) {
	specialTimer_ -= deltaTime;
	specialHitInterval_ -= deltaTime;

	// 一定間隔で敵のところにワープして斬る
	if (specialHitInterval_ <= 0.0f) {

		// 生きている敵がいるか確認
		Enemy* target = nullptr;

		int aliveCount = 0;
		for (Enemy* e : enemies_) {
			if (e && !e->IsDead()) {
				aliveCount++;
			}
		}

		if (aliveCount > 0) {
			// ランダムな敵を1体選ぶ
			int targetIndex = rand() % aliveCount;
			int current = 0;
			for (Enemy* e : enemies_) {
				if (e && !e->IsDead()) {
					if (current == targetIndex) {
						target = e;
						break;
					}
					current++;
				}
			}
		}

		if (target) {
			Vector3 enemyPos = target->GetWorldTransform().translation_;

			// 敵の周囲のどこかにワープ
			float side = (rand() % 2 == 0) ? -1.0f : 1.0f;
			Vector3 warpPos = enemyPos;
			warpPos.x += side * 1.0f;
			warpPos.y += 0.2f;

			player_->WarpTo(warpPos);

			// 次に斬るまでの間隔
			specialHitInterval_ = 0.06f;
		}
	}

	// 規定時間が終わったら終わる
	if (specialTimer_ <= 0.0f) {
		specialState_ = SpecialState::Finish;
		specialTimer_ = 0.0f; // 画面全体の斬撃を見せる時間
	}
}

// ==========================================================================
// 描画処理
// ==========================================================================

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

	// DeathParticle描画
	deathParticle_.Draw();

	for (const auto& vfx : SlashEffects_) {
		vfx->Draw();
	}

	modelHp_->Draw(worldTransformHudHp_, *uiCamera_);

	modelHpBar_->Draw(worldTransformHudHpBar_, *uiCamera_);

	comboRank_.Draw();

	Model::PostDraw();

	if (fade_) {
		fade_->Draw();
	}
}

// ==========================================================================
// 解放処理
// ==========================================================================

GameScene::~GameScene() {
	delete modelPlayer_;
	delete modelEnemy_;
	delete modelDeathParticle_;
	delete modelBullet_;
	delete model_;
	delete modelCube_;
	delete modelSkyDome_;
	delete modelPlayer_;
	delete modelEnemy_;
	delete modelDeathParticle_;
	delete modelBullet_;
	delete modelHp_;
	delete modelHpBar_;
	delete debugCamera_;
	delete cameraController_;
	delete modelSlowBall_;
	delete mapChipField_;
	delete modelZangeki_;
	delete player_;
	delete fade_;
	delete camera_;
	delete uiCamera_;

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
