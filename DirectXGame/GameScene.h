#pragma once
#include "KamataEngine.h"
#include <vector>

using namespace KamataEngine;

class GameScene {
public:
	// デストラクタ
	~GameScene();

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

private:
	// modelハンドル
	Model* modelBlock_ = nullptr;

	// スプライト
	Model* model_ = nullptr;

	// ワールドトランスフォーム
	WorldTransform worldTransform_;

	// カメラ
	Camera* camera_;
	bool isDebugCameraActive = false;
	
	// デバッグカメラ
	DebugCamera* debugCamera_ = nullptr;

	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;

};
