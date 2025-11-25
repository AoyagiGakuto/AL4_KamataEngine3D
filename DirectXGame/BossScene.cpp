#include "BossScene.h"
#include "MyMath.h"
#include <algorithm>
#include <numbers>

using namespace KamataEngine;
/*
void BossScene::Initialize() {
	modelCube_ = Model::CreateFromOBJ("block");
	modelSkyDome_ = Model::CreateFromOBJ("SkyDome", true);
	modelPlayer_ = Model::CreateFromOBJ("player");
	modelBoss_ = Model::CreateFromOBJ("Ninja");
	modelBullet_ = Model::CreateFromOBJ("bullet");

	worldTransform_.Initialize();

	// マップ
	mapChipField_ = new MapChipField();
	// ボス部屋用のCSVを用意
	mapChipField_->LoadMapChipCsv("Resources/boss_room.csv");

	GenerateBlocks();

	// カメラ
	camera_ = new Camera();
	camera_->Initialize();

	// プレイヤー
	player_ = new Player();
	Vector3 playerPos = mapChipField_->GetMapPositionTypeByIndex(10, 18);
	player_->Initialize(modelPlayer_, camera_, playerPos);
	player_->SetMapChipField(mapChipField_);

	// ボス
	boss_ = new Boss();
	Vector3 bossPos = mapChipField_->GetMapPositionTypeByIndex(45, 18);
	boss_->Initialize(modelBoss_ ? modelBoss_ : modelCube_, camera_, bossPos);

	// カメラコントローラ
	cameraController_ = new CameraController();
	cameraController_->SetTarget(player_);
	CameraController::Rect area = {12.0f, mapChipField_->GetNumBlockHorizontal() - 12.0f, 6.0f, 6.0f};
	cameraController_->SetMovableArea(area);
	cameraController_->Initialize();
	cameraController_->Reset();

	// 死亡パーティクル
	Model* modelDeathParticle = Model::CreateFromOBJ("deathParticle");
	deathParticle_.Initialize(modelDeathParticle, camera_);

	// フェード
	fade_ = new Fade();
	fade_->Initialize();
	phase_ = Phase::kFadeIn;
	fade_->Start(Fade::Status::FadeIn, 1.0f);
}

void BossScene::GenerateBlocks() {
	const uint32_t H = mapChipField_->GetNumBlockVertical();
	const uint32_t W = mapChipField_->GetNumBlockHorizontal();

	worldTransformBlocks_.assign(H, std::vector<WorldTransform*>(W, nullptr));
	for (uint32_t y = 0; y < H; ++y) {
		for (uint32_t x = 0; x < W; ++x) {
			if (mapChipField_->GetMapChipTypeByIndex(x, y) == MapChipType::kBlock) {
				auto* wt = new WorldTransform();
				wt->Initialize();
				wt->translation_ = mapChipField_->GetMapPositionTypeByIndex(x, y);
				worldTransformBlocks_[y][x] = wt;
			}
		}
	}
}

void BossScene::Update() {
	// マップブロック行列更新
	for (auto& line : worldTransformBlocks_) {
		for (auto* block : line) {
			if (!block)
				continue;
			block->matWorld_ = MakeAffineMatrix(block->scale_, block->rotation_, block->translation_);
			block->TransferMatrix();
		}
	}

	// 発射
	if (Input::GetInstance()->TriggerKey(DIK_H)) {
		// 回頭中は撃たない
		if (player_->GetTurnTimer() <= 0.0f) {
			Vector3 spawn = player_->GetWorldTransform().translation_;
			spawn.y += Player::kHeight * 0.5f;
			float y = player_->GetWorldTransform().rotation_.y;
			Vector3 dir = {std::sin(y), 0.0f, -std::cos(y)};

			auto b = std::make_unique<Bullet>();
			b->Initialize(modelBullet_ ? modelBullet_ : modelCube_, camera_, spawn, dir);
			bullets_.push_back(std::move(b));
		}
	}

	// 弾更新
	for (auto& b : bullets_)
		b->Update();
	bullets_.erase(std::remove_if(bullets_.begin(), bullets_.end(), [](const std::unique_ptr<Bullet>& b) { return !b->IsAlive(); }), bullets_.end());

	// プレイヤー更新（死亡してたら止める）
	if (!player_->IsDead()) {
		player_->Update();
		CheckAllCollisions();
	}

	// ボス更新
	if (boss_ && !boss_->IsDead()) {
		boss_->Update(player_);
	}

	// パーティクル
	deathParticle_.Update();

	// フェーズ処理（ボス撃破でフェードアウト→終了）
	switch (phase_) {
	case Phase::kFadeIn:
		fade_->Update();
		if (fade_->IsFinished())
			phase_ = Phase::kPlay;
		break;
	case Phase::kPlay:
		if (boss_ && boss_->IsDead() && deathParticle_.IsFinished()) {
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, 1.0f);
		}
		break;
	case Phase::kFadeOut:
		fade_->Update();
		if (fade_->IsFinished())
			finished_ = true;
		break;
	}

	// カメラ
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
}

void BossScene::Draw() {
	DirectXCommon* dx = DirectXCommon::GetInstance();
	Model::PreDraw(dx->GetCommandList());

	// ブロック
	for (auto& line : worldTransformBlocks_) {
		for (auto* block : line) {
			if (!block)
				continue;
			modelCube_->Draw(*block, *camera_);
		}
	}

	// 空
	modelSkyDome_->Draw(worldTransform_, *camera_);

	// キャラ
	if (boss_ && !boss_->IsDead())
		boss_->Draw();
	if (player_)
		player_->Draw();

	// 弾
	for (auto& b : bullets_)
		b->Draw();

	// 演出
	deathParticle_.Draw();

	Model::PostDraw();
	if (fade_){
		fade_->Draw();
		}
}

BossScene::~BossScene() {
	delete modelCube_;
	delete modelSkyDome_;
	delete modelPlayer_;
	delete modelBoss_;
	delete camera_;
	delete debugCamera_;
	delete cameraController_;
	delete mapChipField_;
	delete player_;
	delete boss_;
	delete fade_;

	for (auto& line : worldTransformBlocks_) {
		for (auto* block : line)
			delete block;
	}
	worldTransformBlocks_.clear();
}
*/