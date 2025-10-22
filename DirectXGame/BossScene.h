#pragma once
#include "Boss.h"
#include "Bullet.h"
#include "CameraController.h"
#include "DeathParticle.h"
#include "Fade.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Player.h"
#include <list>
#include <memory>
#include <vector>

using namespace KamataEngine;
/*
class BossScene {
public:
	~BossScene();

	void Initialize();
	void Update();
	void Draw();

	bool IsFinished() const { return finished_; }

private:
	void GenerateBlocks();
	void CheckAllCollisions();

	enum class Phase { kFadeIn, kPlay, kFadeOut };
	Phase phase_ = Phase::kFadeIn;

	// モデル
	Model* modelCube_ = nullptr;
	Model* modelSkyDome_ = nullptr;
	Model* modelPlayer_ = nullptr;
	Model* modelBoss_ = nullptr;
	Model* modelBullet_ = nullptr;

	// ワールド
	WorldTransform worldTransform_; // スカイドーム用

	// カメラ
	Camera* camera_ = nullptr;
	bool isDebugCameraActive_ = false;
	DebugCamera* debugCamera_ = nullptr;
	CameraController* cameraController_ = nullptr;

	// マップ
	MapChipField* mapChipField_ = nullptr;
	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;

	// キャラ
	Player* player_ = nullptr;
	Boss* boss_ = nullptr;

	// 弾
	std::vector<std::unique_ptr<Bullet>> bullets_;

	// エフェクト/フェード
	DeathParticle deathParticle_;
	Fade* fade_ = nullptr;

	// 状態
	bool finished_ = false;
};
*/