#pragma once
#include "CameraController.h"
#include "DeathParticle.h"
#include "Enemy.h"
#include "Fade.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Player.h"
#include "Skydome.h"
#include <memory>
#include <vector>

using namespace KamataEngine;

class GameScene {
public:
	~GameScene();

	void Initialize();
	void Update();
	void Draw();
	bool IsFinished() const { return finished_; }

private:
	void GenerateBlooks();
	void CheckAllCollisions();

	// フェーズを追加：入る/遊ぶ/出る
	enum class Phase { kFadeIn, kPlay, kFadeOut };
	Phase phase_ = Phase::kFadeIn;

	// モデル
	Model* modelCube_ = nullptr;
	Model* modelSkyDome_ = nullptr;
	Model* model_ = nullptr;
	Model* modelPlayer_ = nullptr;
	Model* modelEnemy_ = nullptr;
	Model* modelDeathParticle_ = nullptr;

	// ワールドトランスフォーム
	WorldTransform worldTransform_;

	// カメラ
	Camera* camera_;
	bool isDebugCameraActive_ = false;
	DebugCamera* debugCamera_ = nullptr;

	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;
	MapChipField* mapChipField_ = nullptr;

	Player* player_ = nullptr;
	std::list<Enemy*> enemies_;
	CameraController* cameraController_ = nullptr;

	// デスパーティクル
	DeathParticle deathParticle_;
	float particleCooldown_ = 0.0f;

	// シーン終了フラグ
	bool finished_ = false;

	// シーン内フェード
	Fade* fade_ = nullptr;
};
