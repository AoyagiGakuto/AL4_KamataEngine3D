#pragma once
#include "KamataEngine.h"
#include <vector>
#include "Skydome.h"
#include "MapChipField.h"
#include "Player.h"
#include "Enemy.h"
#include "CameraController.h"

using namespace KamataEngine;

class GameScene {
public:
	// デストラクタ
	~GameScene();

	// 初期化
	void Initialize();

	// 更新
	void Update();
	void CheckAllCollisions();

	// 描画
	void Draw();

private:

	// modelハンドル
	Model* modelCube_ = nullptr;
	Model* modelSkyDome_ = nullptr;

	// スプライト
	Model* model_ = nullptr;

	Model* modelPlayer_ = nullptr;

	Model* modelEnemy_ = nullptr;

	// ワールドトランスフォーム
	WorldTransform worldTransform_;

	// カメラ
	Camera* camera_;
	bool isDebugCameraActive_ = false;
	
	// デバッグカメラ
	DebugCamera* debugCamera_ = nullptr;

	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;

	MapChipField* mapChipField_;

	void GenerateBlooks();

	Player* player_ = nullptr;

	std::list<Enemy*> enemies_;

	CameraController* cameraController_ = nullptr;

};
