#include "GameScene.h"
#include "CameraController.h"
#include "MyMath.h"
#include <algorithm> // std::max, std::clamp
#include <numbers>
#include <random>

using namespace KamataEngine;

// ランダムユーティリティ
static inline std::mt19937& Rng() {
	static std::random_device rd;
	static std::mt19937 mt(rd());
	return mt;
}
static inline std::string PickRandom(const std::vector<std::string>& v) {
	if (v.empty())
		return "test";
	std::uniform_int_distribution<size_t> d(0, v.size() - 1);
	return v[d(Rng())];
}

void GameScene::Initialize() {
	modelCube_ = Model::CreateFromOBJ("block");
	modelSkyDome_ = Model::CreateFromOBJ("tenkixyuu", true); // 天球
	modelPlayer_ = Model::CreateFromOBJ("player");
	modelEnemy_ = Model::CreateFromOBJ("Ninja");
	modelDeathParticle_ = Model::CreateFromOBJ("deathParticle");

	model_ = Model::Create();
	mapChipField_ = new MapChipField();
	player_ = new Player();

	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");

	// スカイドームWT（中身はUpdateで毎フレ追従）
	worldTransform_.Initialize();

	camera_ = new Camera();
	camera_->Initialize();

	GenerateBlooks();

	// プレイヤー初期位置
	Vector3 playerPosition = mapChipField_->GetMapPositionTypeByIndex(25, 18);
	player_->Initialize(modelPlayer_, camera_, playerPosition);
	player_->SetMapChipField(mapChipField_);

	// ===== 敵スポーン地点 =====
	std::vector<Vector3> spawnPoints;
	{
		auto pos = [&](uint32_t x, uint32_t y) {
			Vector3 p = mapChipField_->GetMapPositionTypeByIndex(x, y);
			p.y -= MapChipField::kBlockHeight / 2.0f;
			return p;
		};
		spawnPoints.push_back(pos(30, 18));
		spawnPoints.push_back(pos(34, 18));
		spawnPoints.push_back(pos(40, 18));
		spawnPoints.push_back(pos(46, 18));
		spawnPoints.push_back(pos(54, 18));
	}

	// 敵生成
	for (auto& p : spawnPoints) {
		Enemy* e = new Enemy();
		e->Initialize(modelEnemy_, camera_, p);
		e->SetMapChipField(mapChipField_);
		e->SetScale({0.4f, 0.4f, 0.4f});
		e->SetRotationY(std::numbers::pi_v<float> * 3.0f / 2.0f);
		enemies_.push_back(e);
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

	// フェード
	fade_ = new Fade();
	fade_->Initialize();
	phase_ = Phase::kFadeIn;
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	// タイピング初期化
	typing_.Initialize(typingTimeLimit_);
	typingTarget_ = nullptr;

	// ★ 単語名 → リソース名のオーバーライド（player だけ playerMoji）
	wordResOverride_["player"] = "playerMoji";

	// 単語OBJのワールド（均等・小さめ）
	wordTransform_.Initialize();
	wordTransform_.scale_ = {0.4f, 0.4f, 0.4f};
	wordTransform_.rotation_ = {0.0f, 0.0f, 0.0f};
	wordTransform_.translation_ = player_->GetWorldTransform().translation_ + typingAnchorOffset_;
	wordTransform_.TransferMatrix();

	// ハイライト（無ければ block にフォールバック）
	modelHighlight_ = Model::CreateFromOBJ("highlight");
	if (!modelHighlight_) {
		modelHighlight_ = modelCube_;
	}

	hlTransform_.Initialize();
	hlBackTransform_.Initialize();
	hlBackTransform_.scale_ = {hlFullWidth_, hlHeight_, 0.02f};
	hlTransform_.scale_ = {0.0f, hlHeight_, 0.12f}; // 厚み↑で確実に表示

	// ===== ゴール =====
	goalTransform_.Initialize();
	goalTransform_.translation_ = mapChipField_->GetMapPositionTypeByIndex(95, 18);
	goalTransform_.scale_ = {1.5f, 2.0f, 1.5f};
	goalTransform_.TransferMatrix();

	// AABB
	float gx = goalTransform_.translation_.x;
	float gy = goalTransform_.translation_.y;
	float gz = goalTransform_.translation_.z;
	goalAabb_.min = {gx - 0.8f, gy - 1.0f, gz - 0.8f};
	goalAabb_.max = {gx + 0.8f, gy + 1.0f, gz + 0.8f};

	mapCleared_ = false;

	// ★ スカイドーム巨大化＆初期配置（継ぎ目を背面へ）
	worldTransform_.scale_ = {1.0f, 1.0f, 0.0f};
	worldTransform_.rotation_ = {0.0f, std::numbers::pi_v<float>, 0.0f};
	worldTransform_.translation_ = player_->GetWorldTransform().translation_;
	worldTransform_.TransferMatrix();
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

	// ===== フェーズ進行 =====
	switch (phase_) {
	case Phase::kFadeIn:
		fade_->Update();
		if (fade_->IsFinished()) {
			phase_ = Phase::kPlay;
		}
		break;

	case Phase::kPlay:
		if (!player_->IsDead()) {
			player_->Update();
			CheckAllCollisions();
			CheckGoalCollision();
		}
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}
		if (player_->IsDead() && deathParticle_.IsFinished()) {
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, 1.0f);
		}
		if (mapCleared_) {
			phase_ = Phase::kClearFadeOut;
			fade_->Start(Fade::Status::FadeOut, 1.0f);
		}
		break;

	case Phase::kTyping: {
		typing_.Update();

		UpdateWordTransformFollowPlayer();

		// 入力割合でハイライト更新
		const size_t total = currentTypingWord_.size();
		const size_t typed = typing_.GetTyped().size();
		float ratio = (total > 0) ? static_cast<float>(typed) / static_cast<float>(total) : 0.0f;

		Vector3 base = wordTransform_.translation_;
		base.y += hlOffsetY_;
		base.z += hlOffsetZ_;

		hlBackTransform_.translation_ = base;
		hlBackTransform_.matWorld_ = MakeAffineMatrix(hlBackTransform_.scale_, hlBackTransform_.rotation_, hlBackTransform_.translation_);
		hlBackTransform_.TransferMatrix();

		float leftGrowOffset = (ratio - 1.0f) * (hlFullWidth_ * 0.5f);
		hlTransform_.translation_ = {base.x + leftGrowOffset, base.y, base.z};
		hlTransform_.scale_.x = (std::max)(0.0f, hlFullWidth_ * ratio);
		hlTransform_.scale_.y = hlHeight_;
		hlTransform_.scale_.z = 0.12f;
		hlTransform_.matWorld_ = MakeAffineMatrix(hlTransform_.scale_, hlTransform_.rotation_, hlTransform_.translation_);
		hlTransform_.TransferMatrix();

		if (typing_.IsSuccess()) {
			if (typingTarget_) {
				AABB eaabb = typingTarget_->GetAABB();
				deathParticle_.Spawn(AabbCenter(eaabb));
				for (auto it = enemies_.begin(); it != enemies_.end(); ++it) {
					if (*it == typingTarget_) {
						delete *it;
						enemies_.erase(it);
						break;
					}
				}
			}
			typingTarget_ = nullptr;
			phase_ = Phase::kPlay;
		} else if (typing_.IsTimeout()) {
			if (!player_->IsDead()) {
				player_->Die();
				deathParticle_.Spawn(player_->GetWorldTransform().translation_);
			}
			typingTarget_ = nullptr;
			phase_ = Phase::kPlay;
		}
	} break;

	case Phase::kFadeOut:
		fade_->Update();
		if (fade_->IsFinished()) {
			finished_ = true;
		}
		break;

	case Phase::kClearFadeOut:
		fade_->Update();
		if (fade_->IsFinished()) {
			finished_ = true;
		}
		break;
	}

	// 演出更新
	deathParticle_.Update();

	// カメラ（debugCamera_ が無い時は常に通常カメラ）
	cameraController_->Update();
	if (debugCamera_ && Input::GetInstance()->TriggerKey(DIK_O)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
	if (debugCamera_ && isDebugCameraActive_) {
		debugCamera_->Update();
		camera_->matView = debugCamera_->GetCamera().matView;
		camera_->matProjection = debugCamera_->GetCamera().matProjection;
	} else {
		camera_->matView = cameraController_->GetViewProjection().matView;
		camera_->matProjection = cameraController_->GetViewProjection().matProjection;
		camera_->TransferMatrix();
	}

	// ★ スカイドーム：毎フレでプレイヤー位置に追従（巨大化＆継ぎ目は背面）
	{
		const Vector3 p = player_->GetWorldTransform().translation_;
		worldTransform_.translation_ = p;
		worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
		worldTransform_.TransferMatrix();
	}
}

void GameScene::CheckAllCollisions() {
	if (player_->IsDead())
		return;

	AABB aabb1 = player_->GetAABB();

	for (Enemy* enemy : enemies_) {
		AABB aabb2 = enemy->GetAABB();
		bool isHit = (aabb1.min.x < aabb2.max.x && aabb1.max.x > aabb2.min.x) && (aabb1.min.y < aabb2.max.y && aabb1.max.y > aabb2.min.y) && (aabb1.min.z < aabb2.max.z && aabb1.max.z > aabb2.min.z);

		if (isHit) {
			// 必要な1個だけロード
			std::string word = PickRandom(typingWords_);
			currentTypingWord_ = word;
			typing_.Start(word, typingTimeLimit_);

			// 単語の幅（ざっくり文字数で）調整
			hlFullWidth_ = std::clamp(0.45f * static_cast<float>(currentTypingWord_.size()), 3.0f, 10.0f);
			hlBackTransform_.scale_.x = hlFullWidth_;

			LoadWordModel(currentTypingWord_);
			typingTarget_ = enemy;
			phase_ = Phase::kTyping;
			break;
		}
	}
}

void GameScene::CheckGoalCollision() {
	AABB pa = player_->GetAABB();
	bool hit = (pa.min.x < goalAabb_.max.x && pa.max.x > goalAabb_.min.x) && (pa.min.y < goalAabb_.max.y && pa.max.y > goalAabb_.min.y) && (pa.min.z < goalAabb_.max.z && pa.max.z > goalAabb_.min.z);
	if (hit) {
		mapCleared_ = true;
	}
}

void GameScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	Model::PreDraw(dxCommon->GetCommandList());

	// マップ
	for (auto& line : worldTransformBlocks_) {
		for (auto& block : line) {
			if (!block)
				continue;
			modelCube_->Draw(*block, *camera_);
		}
	}

	// スカイドーム（以前と同じ順。これで見え方が安定）
	modelSkyDome_->Draw(worldTransform_, *camera_);

	// タイピング中：ハイライト→単語OBJ
	if (phase_ == Phase::kTyping) {
		if (modelHighlight_)
			modelHighlight_->Draw(hlBackTransform_, *camera_);
		else
			modelCube_->Draw(hlBackTransform_, *camera_);

		if (modelHighlight_)
			modelHighlight_->Draw(hlTransform_, *camera_);
		else
			modelCube_->Draw(hlTransform_, *camera_);

		auto it = wordCache_.find(currentTypingWord_);
		if (it != wordCache_.end() && it->second) {
			it->second->Draw(wordTransform_, *camera_);
		}
	}

	// ゴール
	modelCube_->Draw(goalTransform_, *camera_);

	// キャラクター
	player_->Draw();
	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}

	// デスパーティクル
	deathParticle_.Draw();

	Model::PostDraw();

	if (fade_)
		fade_->Draw();
}

Model* GameScene::LoadWordModel(const std::string& word) {
	if (auto it = wordCache_.find(word); it != wordCache_.end()) {
		return it->second;
	}
	std::string resName;
	if (auto ov = wordResOverride_.find(word); ov != wordResOverride_.end()) {
		resName = ov->second; // 例: player -> playerMoji
	} else {
		resName = wordPrefix_ + word + wordSuffix_;
	}
	Model* m = Model::CreateFromOBJ(resName.c_str());
	if (!m) {
		std::string alt = word + std::string("Moji");
		m = Model::CreateFromOBJ(alt.c_str());
		if (!m) {
			alt = std::string("moji_") + word;
			m = Model::CreateFromOBJ(alt.c_str());
		}
	}
	wordCache_[word] = m;
	return m;
}

void GameScene::UpdateWordTransformFollowPlayer() {
	Vector3 base = player_->GetWorldTransform().translation_;
	wordTransform_.translation_.x = base.x + typingAnchorOffset_.x;
	wordTransform_.translation_.y = base.y + typingAnchorOffset_.y;
	wordTransform_.translation_.z = base.z + typingAnchorOffset_.z;
	wordTransform_.matWorld_ = MakeAffineMatrix(wordTransform_.scale_, wordTransform_.rotation_, wordTransform_.translation_);
	wordTransform_.TransferMatrix();
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
