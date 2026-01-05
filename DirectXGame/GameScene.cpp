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
	modelEnemy_ = Model::CreateFromOBJ("enemyMonster");
	modelDeathParticle_ = Model::CreateFromOBJ("deathParticle");
	modelChargeParticle_ = Model::CreateFromOBJ("chargeParticle");
	modelBullet_ = Model::CreateFromOBJ("bullet");
	modelSlowBall_ = Model::CreateFromOBJ("bullet");
	modelZangeki_ = Model::CreateFromOBJ("zangeki");
	modelHp_ = Model::CreateFromOBJ("hp");
	modelHpBar_ = Model::CreateFromOBJ("hpbar");
	tutorialModel_ = Model::CreateFromOBJ("setumei");
	tKeyModel_ = Model::CreateFromOBJ("Tkey");

	for (int i = 0; i < 10; ++i) {
		modelNumbers_[i] = Model::CreateFromOBJ(std::to_string(i));
	}
	
	// delete忘れないように

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
	
	// カメラの生成
	Matrix4x4 matWorld = MakeAffineMatrix(uiScale, uiRot, uiPos);
	uiCamera_->matView = Inverse(matWorld);
	uiCamera_->TransferMatrix();

	// チュートリアルというか説明
	tutorialWT_.Initialize();
	tutorialWT_.translation_ = {0.0f, -0.6f, -2.0f};
	tutorialWT_.scale_ = {0.4f, 0.4f, 0.4f};
	
	tKeyWT_.Initialize();
	tKeyWT_.scale_ = {0.5f, 0.5f, 0.5f};

	// HPバーの下あたりに
	tKeyWT_.translation_ = {-5.0f, 3.0f, 0.0f};
	tKeyColor_.Initialize();
	tKeyColor_.SetColor({0.0f, 0.0f, 0.0f, 1.0f});

	isTutorialMode_ = false;

	// HPバー初期化
	worldTransformHudHpBar_.Initialize();
	worldTransformHudHp_.Initialize();

	// UI位置
	comboRank_.Initialize(uiCamera_, GameParam::kComboPos, modelHpBar_, modelHp_);

	worldTransformHudHpBar_.translation_ = GameParam::kHudPos;
	worldTransformHudHp_.translation_ = GameParam::kHudPos;
	worldTransformHudHpBar_.scale_ = GameParam::kHudScale;
	worldTransformHudHp_.scale_ = GameParam::kHudScale;

	// ブロック生成
	GenerateBlocks();

	// プレイヤー初期位置
	Vector3 playerPosition = mapChipField_->GetMapPositionTypeByIndex(25, 18);
	player_->Initialize(modelPlayer_, camera_, playerPosition);
	player_->SetMapChipField(mapChipField_);

	// 敵の初期配置
	/*
	Enemy* firstEnemy = new NormalEnemy();
	Vector3 enemyPos = mapChipField_->GetMapPositionTypeByIndex(30, 18);
	firstEnemy->Initialize(modelEnemy_, modelHpBar_, modelHp_, camera_, enemyPos);
	firstEnemy->SetScale({GameParam::kEnemyScaleNormal, GameParam::kEnemyScaleNormal, GameParam::kEnemyScaleNormal});
	firstEnemy->SetTarget(player_);
	firstEnemy->SetMapChipField(mapChipField_);
	firstEnemy->SetRotationY(std::numbers::pi_v<float> * 3.0f / 2.0f);
	enemies_.push_back(firstEnemy);
	*/

	// カメラコントローラー
	cameraController_ = new CameraController();
	cameraController_->SetTarget(player_);
	CameraController::Rect cameraArea = {12.0f, 100 - 12.0f, 6.0f, 6.0f};
	cameraController_->SetMovableArea(cameraArea);
	cameraController_->Initialize();
	cameraController_->Reset();

	deathParticle_.Initialize(modelDeathParticle_, camera_);

	hasUsedSpecial_ = false;
	chargeParticles_.clear();

	particleCooldown_ = 0.0f;
	score_ = 0;
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	// サバイバルタイマー初期化
	survivalTimer_ = GameParam::kSurvivalTimeLimit;
	enemySpawnTimer_ = 0.0f; // すぐに湧くように
	isClear_ = false;

	// タイマー用ワールド変換の初期化
	for (auto& wt : timerWTs_) {
		wt.Initialize();
		wt.rotation_.y = std::numbers::pi_v<float> / -2.0f;
		// 少し大きめに表示
		wt.scale_ = {2.0f, 2.0f, 2.0f};
	}

	// スコア用ワールド変換の初期化
	for (auto& wt : scoreWTs_) {
		wt.Initialize();
		wt.rotation_.y = std::numbers::pi_v<float> / -2.0f; // 正面に向ける
		wt.scale_ = {1.5f, 1.5f, 1.5f};                     // 少し小さめ
	}

	srand((unsigned int)time(NULL));
}

// ==========================================================================
// ブロック生成
// ==========================================================================
void GameScene::GenerateBlocks() {
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

	// Tキーでポーズ切り替え
	if (Input::GetInstance()->TriggerKey(DIK_T)) {
		isTutorialMode_ = !isTutorialMode_;
	}

	tKeyWT_.matWorld_ = MakeAffineMatrix(tKeyWT_.scale_, tKeyWT_.rotation_, tKeyWT_.translation_);
	tKeyWT_.TransferMatrix();

	// 説明が出てきているならゲームを止める
	if (isTutorialMode_) {
		tutorialWT_.matWorld_ = MakeAffineMatrix(tutorialWT_.scale_, tutorialWT_.rotation_, tutorialWT_.translation_);
		tutorialWT_.TransferMatrix();
		return;
	}

	const float dt = 1.0f / 60.0f;

	// サバイバルタイマー更新
	if (!isClear_ && !player_->IsDead()) {
		survivalTimer_ -= dt;
		// 時間経過でクリア
		if (survivalTimer_ <= 0.0f) {
			isClear_ = true;
			// 敵を全消去してフェードアウト開始
			phase_ = ScenePhase::FadeOut;
			fade_->Start(Fade::Status::FadeOut, 1.0f);
		}
	}

	// 敵のスポーン処理
	UpdateEnemySpawn();

	if (comboTimer_ > 0.0f) {
		comboTimer_ -= dt;
		if (comboTimer_ <= 0.0f) {
			comboIndex_ = 0;
		}
	}

	if (particleCooldown_ > 0.0f) {
		particleCooldown_ -= dt;
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

	UpdateSpecialMove(dt);
	UpdateMapBlocks();
	UpdatePlayerAction();

	if (!player_->IsDead()) {
		if (specialState_ == SpecialState::None) {
			player_->Update();
			CheckAllCollisions();
		}
	}

	UpdateEnemies();
	UpdateProjectiles();
	CheckCollisions();

	deathParticle_.Update();

	UpdateChargeParticles();

	for (auto& vfx : slashEffects_) {
		vfx->Update();
	}

	slashEffects_.erase(std::remove_if(slashEffects_.begin(), slashEffects_.end(), [](const std::unique_ptr<SlashEffect>& v) { return !v->IsAlive(); }), slashEffects_.end());

	UpdateHud();
	UpdateSceneFlow();
}

// ==========================================================================
// 敵スポーン処理
// ==========================================================================
void GameScene::UpdateEnemySpawn() {
	if (isClear_ || player_->IsDead()) {
		return;
	}

	enemySpawnTimer_ -= 1.0f / 60.0f;

	if (enemySpawnTimer_ <= 0.0f) {
		enemySpawnTimer_ = GameParam::kEnemySpawnInterval;

		// ステージの幅（ブロック数）
		uint32_t mapWidth = mapChipField_->GetNumBlockHorizontal();

		// ランダムなX座標
		uint32_t randomX = rand() % (mapWidth - 10) + 5;
		// 高さ
		uint32_t spawnY = 15;

		Vector3 spawnPos = mapChipField_->GetMapPositionTypeByIndex(randomX, spawnY);

		// 敵の種類をランダムに決定 (0ふつう, 1ついてくる敵, 2飛んでる敵)
		int enemyType = rand() % 3;
		Enemy* newEnemy = nullptr;

		switch (enemyType) {
		case 0:
			newEnemy = new NormalEnemy();
			newEnemy->Initialize(modelEnemy_, modelHpBar_, modelHp_, camera_, spawnPos);
			newEnemy->SetScale({GameParam::kEnemyScaleNormal, GameParam::kEnemyScaleNormal, GameParam::kEnemyScaleNormal});
			break;
		case 1:
			newEnemy = new HomingEnemy();
			newEnemy->Initialize(modelEnemy_, modelHpBar_, modelHp_, camera_, spawnPos);
			newEnemy->SetScale({GameParam::kEnemyScaleNormal, GameParam::kEnemyScaleNormal, GameParam::kEnemyScaleNormal});
			break;
		case 2:
			newEnemy = new FlyingEnemy();
			newEnemy->Initialize(modelEnemy_, modelHpBar_, modelHp_, camera_, spawnPos);
			newEnemy->SetScale({GameParam::kEnemyScaleSmall, GameParam::kEnemyScaleSmall, GameParam::kEnemyScaleSmall});
			break;
		}

		if (newEnemy) {
			newEnemy->SetTarget(player_);
			newEnemy->SetMapChipField(mapChipField_);
			// プレイヤーの方を向かせる
			if (player_->GetPosition().x < spawnPos.x) {
				newEnemy->SetRotationY(std::numbers::pi_v<float> * 3.0f / 2.0f);
			} else {
				newEnemy->SetRotationY(std::numbers::pi_v<float> / 2.0f);
			}
			enemies_.push_back(newEnemy);
		}
	}
}

// ==========================================================================
// マップブロックの更新
// ==========================================================================
void GameScene::UpdateMapBlocks() {
	for (auto& line : worldTransformBlocks_) {
		for (auto& block : line) {
			if (!block) {
				continue;
			}

			block->matWorld_ = MakeAffineMatrix(block->scale_, block->rotation_, block->translation_);
			block->TransferMatrix();
		}
	}
}

// ==========================================================================
// 入力処理の更新
// ==========================================================================
void GameScene::UpdatePlayerAction() {

	/* --- 通常弾発射 --- */
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
	/* ---スロー弾発射--- */
	if (Input::GetInstance()->TriggerKey(DIK_Y)) {
		if (!player_->IsDead() && player_->IsLockedOn()) {
			Enemy* targetEnemy = player_->GetTargetEnemy();
			if (targetEnemy && !targetEnemy->IsDead()) {
				const float kRainAreaWidth = 5.0f;
				const float kRainHeight = 10.0f;
				float centerX = targetEnemy->GetWorldTransform().translation_.x;
				for (int i = 0; i < GameParam::kRainBallCount; ++i) {
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
	/* ---ロックオン制御--- */
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
	/* ---近接攻撃--- */
	if (Input::GetInstance()->TriggerKey(DIK_K)) {
		if (player_->IsLockedOn()) {
			Enemy* target = player_->GetTargetEnemy();
			if (target && !target->IsDead()) {
				float distance = Length(target->GetWorldTransform().translation_ - player_->GetWorldTransform().translation_);
				if (distance <= GameParam::kPlayerMeleeRange * 1.5f) {
					bool isFacingRight = (player_->GetWorldTransform().rotation_.y < 3.14f);
					bool isBackInput = false;
					if (isFacingRight && Input::GetInstance()->PushKey(DIK_A))
						isBackInput = true;
					if (!isFacingRight && Input::GetInstance()->PushKey(DIK_D))
						isBackInput = true;
					bool isPlayerAir = (player_->GetWorldTransform().translation_.y > 2.0f);
					// 打ち上げ
					if (!isPlayerAir && isBackInput) {
						target->Launch(0.35f);
						player_->velocity_.y = 0.35f;
						target->TakeDamage(GameParam::kDamageNormal);
						hitStopTimer_ = 0.1f;
					}
					// 叩きつけ
					else if (isPlayerAir && isBackInput) {
						target->SlamDown();
						player_->velocity_.y = -0.5f;
						target->TakeDamage(GameParam::kDamageNormal * 2);
						hitStopTimer_ = 0.15f;
					}
					// 空中コンボ
					else if (isPlayerAir) {
						target->OnAirHit(0.4f);
						player_->velocity_.y = 0.08f;
						player_->velocity_.x = 0.0f;
						target->TakeDamage(GameParam::kDamageNormal);
						hitStopTimer_ = 0.08f;
					}
					// 地上通常攻撃
					else {
						comboIndex_++;
						comboTimer_ = 1.0f;
						if (comboIndex_ > 3)
							comboIndex_ = 1;
						Vector3 dir = target->GetWorldTransform().translation_ - player_->GetWorldTransform().translation_;
						dir = Normalize(dir);
						switch (comboIndex_) {
						case 1:
							player_->velocity_.x = dir.x * 0.15f;
							target->Knockback(dir * 0.5f);
							target->TakeDamage(GameParam::kDamageNormal);
							hitStopTimer_ = 0.05f;
							break;
						case 2:
							player_->velocity_.x = dir.x * 0.2f;
							target->Knockback(dir * 0.5f);
							target->TakeDamage(GameParam::kDamageNormal);
							hitStopTimer_ = 0.05f;
							break;
						case 3:
							player_->velocity_.x = dir.x * 0.4f;
							target->Launch(0.2f);
							target->Knockback(dir * 3.0f);
							target->TakeDamage(GameParam::kDamageNormal * 2);
							hitStopTimer_ = 0.2f;
							comboIndex_ = 0;
							comboTimer_ = 0.0f;
							break;
						}
						comboRank_.AddHit(GameParam::kComboPointHit);
					}
					comboRank_.AddHit(GameParam::kComboPointHit);
					if (target->IsDead()) {
						AddScore();
						comboRank_.OnEnemyKilled(GameParam::kComboPointKill);
						deathParticle_.Spawn(target->GetWorldTransform().translation_);
						enemies_.remove(target);
						delete target;
						player_->LockOff();
					}
				}
			}
		}
	}
	/* ---チャージ攻撃--- */
	if (player_->IsChargeAttackReady()) {
		player_->ConsumeChargeAttack();
		Enemy* target = player_->GetTargetEnemy();
		if (target && !target->IsDead()) {
			comboRank_.AddHit(GameParam::kComboPointCharge);
			for (int i = 0; i < GameParam::kChargeAttackHitCount; ++i) {
				target->TakeDamage(GameParam::kDamageNormal);
			}
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
				slashEffects_.push_back(std::move(vfx));
			}
			if (target->IsDead()) {
				AddScore();
				comboRank_.OnEnemyKilled(GameParam::kComboPointKill);
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
	for (auto& b : bullets_) {
		b->Update();
	}
	bullets_.erase(std::remove_if(bullets_.begin(), bullets_.end(), [](const std::unique_ptr<Bullet>& b) { return !b->IsAlive(); }), bullets_.end());

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
		enemy->PerformUniqueAction(enemies_);
		if (enemy->IsReadyToFire()) {
			Vector3 startPos = enemy->GetWorldTransform().translation_;
			Vector3 targetPos = player_->GetWorldTransform().translation_;
			targetPos.y += 0.5f;
			Vector3 dir = targetPos - startPos;
			dir = Normalize(dir);
			auto newBullet = std::make_unique<Bullet>();
			newBullet->Initialize(modelBullet_, camera_, startPos, dir);
			newBullet->SetType(Bullet::Type::kEnemy);
			bullets_.push_back(std::move(newBullet));
		}
	}
}

// ==========================================================================
// 当たり判定処理
// ==========================================================================
void GameScene::CheckCollisions() {
	/* ---通常弾と敵--- */
	for (auto it = bullets_.begin(); it != bullets_.end();) {
		bool bulletRemoved = false;
		if ((*it)->GetType() == Bullet::Type::kEnemy) {
			AABB aabbB = (*it)->GetAABB();
			AABB aabbP = player_->GetAABB();
			bool isHit =
			    (aabbB.min.x < aabbP.max.x && aabbB.max.x > aabbP.min.x) && (aabbB.min.y < aabbP.max.y && aabbB.max.y > aabbP.min.y) && (aabbB.min.z < aabbP.max.z && aabbB.max.z > aabbP.min.z);
			if (isHit) {
				player_->TakeDamage(GameParam::kDamageNormal);
				comboRank_.OnPlayerDamaged();
				(*it)->Kill();
				bulletRemoved = true;
			}
		} else {
			AABB aabbB = (*it)->GetAABB();
			auto enemyIt = enemies_.begin();
			while (enemyIt != enemies_.end()) {
				Enemy* enemy = *enemyIt;
				AABB aabbE = enemy->GetAABB();
				bool isHit =
				    (aabbB.min.x < aabbE.max.x && aabbB.max.x > aabbE.min.x) && (aabbB.min.y < aabbE.max.y && aabbB.max.y > aabbE.min.y) && (aabbB.min.z < aabbE.max.z && aabbB.max.z > aabbB.min.z);
				if (isHit) {
					enemy->TakeDamage(GameParam::kDamageNormal);
					comboRank_.AddHit(5.0f);
					if (enemy->IsDead()) {
						AddScore();
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
		}
		if (bulletRemoved) {
			it = bullets_.erase(it);
			continue;
		}
		MapChipField::IndexSet idx = mapChipField_->GetMapChipIndexSetByPosition((*it)->GetAABB().min);
		MapChipType type = mapChipField_->GetMapChipTypeByIndex(idx.xIndex, idx.yIndex);
		if (type == MapChipType::kBlock) {
			(*it)->Kill();
			it = bullets_.erase(it);
		} else {
			++it;
		}
	}
	/* ---スロー弾と敵--- */
	for (auto it = slowBalls_.begin(); it != slowBalls_.end();) {
		bool ballRemoved = false;
		AABB aabbB = (*it)->GetAABB();
		for (Enemy* enemy : enemies_) {
			AABB aabbE = enemy->GetAABB();
			bool isHit =
			    (aabbB.min.x < aabbE.max.x && aabbB.max.x > aabbE.min.x) && (aabbB.min.y < aabbE.max.y && aabbB.max.y > aabbE.min.y) && (aabbB.min.z < aabbE.max.z && aabbB.max.z > aabbB.min.z);
			if (isHit) {
				enemy->SlowDown(GameParam::kSlowDuration);
				(*it)->Kill();
				ballRemoved = true;
				break;
			}
		}
		if (ballRemoved) {
			it = slowBalls_.erase(it);
			continue;
		}
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

	float baseScaleX = GameParam::kHudScale.x;
	float baseScaleY = GameParam::kHudScale.y;

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
		// プレイヤー死亡時はGameOver
		if (player_->IsDead() && deathParticle_.IsFinished()) {
			phase_ = ScenePhase::FadeOut;
			isClear_ = false;
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
	if (player_->IsDead())
		return;
	AABB aabb1 = player_->GetAABB();
	for (Enemy* enemy : enemies_) {
		if (enemy->IsDead())
			continue;
		AABB aabb2 = enemy->GetAABB();
		bool isHit = (aabb1.min.x < aabb2.max.x && aabb1.max.x > aabb2.min.x) && (aabb1.min.y < aabb2.max.y && aabb1.max.y > aabb2.min.y) && (aabb1.min.z < aabb2.max.z && aabb1.max.z > aabb2.min.z);
		if (isHit) {
			player_->TakeDamage(GameParam::kDamageNormal);
			comboRank_.OnPlayerDamaged();
			Vector3 pPos = player_->GetWorldTransform().translation_;
			Vector3 ePos = enemy->GetWorldTransform().translation_;
			Vector3 dir = pPos - ePos;
			dir = Normalize(dir);
			player_->Knockback(dir);
			enemy->Knockback(dir * -1.0f);
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
	if (specialState_ == SpecialState::None) {
		if (!hasUsedSpecial_ && !player_->IsDead() && Input::GetInstance()->TriggerKey(DIK_R)) {
			hasUsedSpecial_ = true;
			specialState_ = SpecialState::Charge;
			specialTimer_ = GameParam::kSpecialChargeTime;
			specialFinalSlashesSpawned_ = false;
		}
	}
	switch (specialState_) {
	case SpecialState::None:
		break;
	case SpecialState::Charge:
		specialTimer_ -= deltaTime;
		if (specialTimer_ > 0.0f) {
			for (int i = 0; i < 2; i++) {
				auto p = std::make_unique<ChargeParticle>();
				p->transform.Initialize();

				Vector3 center = player_->GetWorldTransform().translation_;

				// ランダム
				Vector3 offset;
				offset.x = (rand() % 200 - 100) / 100.0f;
				offset.y = (rand() % 200 - 100) / 100.0f;
				offset.z = (rand() % 200 - 100) / 100.0f;
				if (Length(offset) != 0.0f) {
					offset = Normalize(offset) * 5.0f;
				}

				p->transform.translation_ = center + offset;
				p->transform.scale_ = {0.5f, 0.5f, 0.5f}; // 少し小さめ

				// 中心（プレイヤー）に向かって吸い込まれる速度
				Vector3 dir = center - p->transform.translation_;
				p->velocity = Normalize(dir) * 0.2f; // 吸い込みスピード

				p->lifeTimer = 60.0f;

				chargeParticles_.push_back(std::move(p));
			}
		}
		if (specialTimer_ <= 0.0f) {
			specialState_ = SpecialState::Dash;
			specialTimer_ = GameParam::kSpecialDashTime;
			specialHitInterval_ = 0.0f;
		}
		break;
	case SpecialState::Dash:
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
				Vector3 enemyPos = e->GetWorldTransform().translation_;
				enemyPos.y += 0.5f;
				e->TakeDamage(GameParam::kDamageSpecial);
				comboRank_.AddHit(GameParam::kComboPointHit);
				if (e->IsDead()) {
					AddScore();
					comboRank_.OnEnemyKilled(GameParam::kComboPointKill);
					deathParticle_.Spawn(enemyPos);
					delete e;
					it = enemies_.erase(it);
				} else {
					++it;
				}
			}
			const int kNumSlashes = GameParam::kSpecialFinaleSlashCount;
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
				slashEffects_.push_back(std::move(vfx));
			}
		}
		if (specialTimer_ <= 0.0f) {
			specialState_ = SpecialState::None;
		}
		break;
	}
}

void GameScene::UpdateChargeParticles() {
	auto it = chargeParticles_.begin();
	while (it != chargeParticles_.end()) {
		ChargeParticle* cp = it->get();
		cp->transform.translation_ += cp->velocity;
		cp->lifeTimer -= 1.0f;
		cp->transform.matWorld_ = MakeAffineMatrix(cp->transform.scale_, cp->transform.rotation_, cp->transform.translation_);
		cp->transform.TransferMatrix();

		Vector3 playerPos = player_->GetWorldTransform().translation_;
		float dist = Length(cp->transform.translation_ - playerPos);

		if (dist < 0.5f || cp->lifeTimer <= 0.0f) {
			it = chargeParticles_.erase(it);
		} else {
			++it;
		}
	}
}

// ==========================================================================
// 必殺技の時のダッシュの実装
// ==========================================================================
void GameScene::PerformSpecialDash(float deltaTime) {
	specialTimer_ -= deltaTime;
	specialHitInterval_ -= deltaTime;
	if (specialHitInterval_ <= 0.0f) {
		Enemy* target = nullptr;
		int aliveCount = 0;
		for (Enemy* e : enemies_) {
			if (e && !e->IsDead())
				aliveCount++;
		}
		if (aliveCount > 0) {
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
			float side = (rand() % 2 == 0) ? -1.0f : 1.0f;
			Vector3 warpPos = enemyPos;
			warpPos.x += side * 1.0f;
			warpPos.y += 0.2f;
			player_->WarpTo(warpPos);
			specialHitInterval_ = GameParam::kSpecialHitInterval;
		}
	}
	if (specialTimer_ <= 0.0f) {
		specialState_ = SpecialState::Finish;
		specialTimer_ = 0.0f;
	}
}

// ==========================================================================
// タイマー描画
// ==========================================================================
void GameScene::DrawTimer() {
	// 残り時間を整数にする (負にならないように0で止める)
	int timeValue = (std::max)(0, (int)survivalTimer_);

	// 文字列に変換
	std::string timeStr = std::to_string(timeValue);

	// 文字の幅（調整用）
	float charWidth = 1.5f;
	// 画面中央に表示するための開始位置計算
	float totalWidth = (timeStr.length() - 1) * charWidth;
	float startX = -totalWidth * 0.5f;

	// 高さ（画面の少し上の方）
	float posY = 2.0f;

	for (size_t i = 0; i < timeStr.length(); ++i) {
		// 文字から数字を取り出す
		int digit = timeStr[i] - '0';

		// 範囲外チェック
		if (digit < 0 || digit > 9)
			continue;

		// 桁ごとの座標計算
		if (i < timerWTs_.size()) {
			timerWTs_[i].translation_ = {startX + i * charWidth, posY, 0.0f};

			// 行列更新
			timerWTs_[i].matWorld_ = MakeAffineMatrix(timerWTs_[i].scale_, timerWTs_[i].rotation_, timerWTs_[i].translation_);
			timerWTs_[i].TransferMatrix();

			// 描画 (UI用のカメラを使う)
			modelNumbers_[digit]->Draw(timerWTs_[i], *uiCamera_);
		}
	}
}

// ==========================================================================
// ランクに応じたスコア加算
// ==========================================================================
void GameScene::AddScore() {
	int point = 1; // デフォルト

	// 現在のランクを取得して点数を変える
	switch (comboRank_.GetRank()) {
	case ComboRank::Rank::D:
		point = 2; // Dランクなら2倍
		break;
	case ComboRank::Rank::C:
		point = 5; // Cランクなら5倍
		break;
	case ComboRank::Rank::B:
		point = 10; // Bランクなら10倍
		break;
	case ComboRank::Rank::A:
		point = 20; // Aランクなら20倍
		break;
	case ComboRank::Rank::S:
		point = 50; // Sランクなら50倍
		break;
	}

	score_ += point;
}

// ==========================================================================
// スコア描画
// ==========================================================================
void GameScene::DrawScore() {
	// スコアを文字列に変換
	std::string scoreStr = std::to_string(score_);

	// 文字の幅
	float charWidth = 1.2f;

	// 表示位置
	float startX = 0.0f;
	float posY = 0.0f;

	// 右揃えで描画するために、文字数分ずらす
	for (int i = 0; i < (int)scoreStr.length(); ++i) {
		int digit = scoreStr[i] - '0';
		if (digit < 0 || digit > 9)
			continue;

		// 配列の範囲内なら描画
		if (i < scoreWTs_.size()) {
			// 位置設定
			scoreWTs_[i].translation_ = {startX + i * charWidth, posY, 0.0f};

			// 行列更新
			scoreWTs_[i].matWorld_ = MakeAffineMatrix(scoreWTs_[i].scale_, scoreWTs_[i].rotation_, scoreWTs_[i].translation_);
			scoreWTs_[i].TransferMatrix();

			// UIカメラで描画
			modelNumbers_[digit]->Draw(scoreWTs_[i], *uiCamera_);
		}
	}
}

// ==========================================================================
// 描画処理
// ==========================================================================
void GameScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	Model::PreDraw(dxCommon->GetCommandList());

	if (tKeyModel_) {
		tKeyModel_->Draw(tKeyWT_, *uiCamera_, &tKeyColor_);
	}

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

	for (const auto& b : bullets_) {
		b->Draw();
	}

	for (const auto& sb : slowBalls_) {
		sb->Draw();
	}

	deathParticle_.Draw();

	for (const auto& p : chargeParticles_) {
		modelChargeParticle_->Draw(p->transform, *camera_);
	}

	for (const auto& vfx : slashEffects_) {
		vfx->Draw();
	}
	modelHp_->Draw(worldTransformHudHp_, *uiCamera_);
	modelHpBar_->Draw(worldTransformHudHpBar_, *uiCamera_);
	comboRank_.Draw();
	DrawTimer();
	DrawScore();

	if (isTutorialMode_ && tutorialModel_) {
		tutorialModel_->Draw(tutorialWT_, *uiCamera_);
	}

	Model::PostDraw();
	if (fade_) {
		fade_->Draw();
	}
}

// ==========================================================================
// 解放処理
// ==========================================================================
GameScene::~GameScene() {
	delete tutorialModel_;
	delete modelPlayer_;
	delete modelEnemy_;
	delete modelDeathParticle_;
	delete modelChargeParticle_;
	delete modelBullet_;
	delete modelSlowBall_;
	delete modelZangeki_;
	delete modelHp_;
	delete modelHpBar_;
	delete model_;
	delete modelCube_;
	delete tKeyModel_;
	delete modelSkyDome_;
	for (Model* model : modelNumbers_) {
		delete model;
	}
	delete player_;
	delete mapChipField_;
	delete debugCamera_;
	delete cameraController_;
	delete camera_;
	delete uiCamera_;
	delete fade_;
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	enemies_.clear();
	for (auto& line : worldTransformBlocks_) {
		for (auto& block : line) {
			delete block;
		}
	}
	worldTransformBlocks_.clear();
}