#pragma once
#include "CameraController.h"
#include "DeathParticle.h"
#include "Enemy.h"
#include "Fade.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Player.h"
#include "Skydome.h"
#include "TypingChallenge.h"
#include <list>
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
	void CheckGoalCollision();
	Vector3 AabbCenter(const AABB& aabb) { return {(aabb.min.x + aabb.max.x) * 0.5f, (aabb.min.y + aabb.max.y) * 0.5f, (aabb.min.z + aabb.max.z) * 0.5f}; }

	// フェーズ：入る/遊ぶ/タイピング/出る/クリア（出る）
	enum class Phase { kFadeIn, kPlay, kTyping, kFadeOut, kClearFadeOut };
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

	// ===== タイピング勝負 =====
	TypingChallenge typing_;
	Enemy* typingTarget_ = nullptr;
	float typingTimeLimit_ = 10.0f;
	// 出題ワード（好きに増やする）
	std::vector<std::string> typingWords_ = {"gundam", "novice", "vector", "matrix", "enemy", "player", "jump", "boost", "lockon", "beam", "funnel", "sword"};

	// ===== ゴール（AABBで判定、見た目はキューブ） =====
	WorldTransform goalTransform_;
	AABB goalAabb_{};
	bool mapCleared_ = false;
};
