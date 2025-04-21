#pragma once
#include "KamataEngine.h"

class Player {
public:
	// 初期化
	void Initialize(Model* model, uint16_t extureHandle_,Camera*camera);
	// 更新
	void Update();
	// 描画
	void Draw();

	Camera* camera_ = nullptr;

private:
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	// モデル
	KamataEngine::Model* model_ = nullptr;
	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;
};
