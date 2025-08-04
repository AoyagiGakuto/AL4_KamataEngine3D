#pragma once
#include "CameraController.h"
#include "DeathParticle.h" // ← 追加
#include "Enemy.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Player.h"
#include "Skydome.h"
#include <memory>
#include <vector>

// CameraController クラスの前方宣言または定義を追加
class CameraController {
public:
	CameraController() = default;
	~CameraController() = default;

	// 必要に応じてメソッドやメンバ変数をここに追加
};

using namespace KamataEngine;

class GameScene {
public:
	~GameScene();

	void Initialize();
	void Update();
	void Draw();

private:
	void GenerateBlooks();
	void CheckAllCollisions();

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
};