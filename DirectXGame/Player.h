#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

class Player {
public:
	// 初期化
	void Initialize(Model* model, Camera* camera, const Vector3& position);
	// 更新
	void Update();
	// 描画
	void Draw();

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* modelPlayer_ = nullptr;
	// カメラ
	Camera* camera_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	// 移動速度
	Vector3 velocity_ = {};

	// 加速度定数
	float kAcceleration = 0.1f;

};
