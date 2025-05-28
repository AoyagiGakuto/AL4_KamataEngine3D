#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

enum class LRDirection {
	kLeft,  // 左
	kRight, // 右
};

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
	Model* model_ = nullptr;
	// 地面にいるかどうかのフラグ
	bool OnGround_ = true;
	// カメラ
	Camera* camera_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	// 移動速度
	Vector3 velocity_ = {};

	// 加速度定数
	float kAcceleration = 0.1f;

	LRDirection lrDirection_ = LRDirection::kRight; // 左右方向の初期値
	float turnFirstRotationY_ = 0.0f;                // 最初の回転角度
	float turnTimer_ = 0.0f;                        // 回転タイマー
	static inline const float kTimeTurn = 0.3f;     // 回転にかかる時間
	static inline const float kGravityAcceleration = 0.01f; // 重力加速度
	static inline const float kLimitFallSpeed = 0.5f;       // 落下速度の制限値
	static inline const float kJumpAcceleration = 0.3f;     // ジャンプ加速度
};
