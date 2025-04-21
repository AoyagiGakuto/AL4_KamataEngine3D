#pragma once
#include "KamataEngine.h"
#include "Player.h"

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

	Player* player_ = nullptr;

private:
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;

	// スプライト
	Model* model_ = nullptr;

	// ワールドトランスフォーム
	WorldTransform worldTransform_;

	// カメラ
	Camera* camera_;
};
