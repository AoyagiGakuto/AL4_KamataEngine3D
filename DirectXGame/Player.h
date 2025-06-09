#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

class MapChipField;

enum class LRDirection {
	kRight, // 右
	kLeft,  // 左
};

struct CollisionMapInfo {
	// 衝突しているかどうか
	bool isCollision = false; 
	// 着地しているかどうか
	bool isOnGround = false;
	// 壁に当たったかどうか
	bool isHitWall = false;
	// 移動量
	Vector3 move = {0.0f, 0.0f, 0.0f};
};

enum Corner {
	kRightBottom, // 右下
	kLeftBottom,  // 左下
	kRightTop,    // 右上
	kLeftTop,     // 左上
	kNumCorner
};

class Player {
public:
	// 初期化
	void Initialize(Model* model, Camera* camera, const Vector3& position);
	// 更新
	void Update();
	// 描画
	void Draw();
	void InputMove(); // 移動入力の処理
	// マップ衝突判定
	void CollisionMapCheck(CollisionMapInfo& Info);
	
	void CheckMapCollisionUp(CollisionMapInfo& Info);

	const WorldTransform& GetWorldTransform() const { return worldTransform_; }        // ワールド変換データの取得
	const Vector3& GetVelocity() const { return velocity_; }                           // 速度の取得
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; } // マップチップフィールドの設定

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

	LRDirection lrDirection_ = LRDirection::kRight;         // 左右方向の初期値
	float turnFirstRotationY_ = 0.0f;                       // 最初の回転角度
	float turnTimer_ = 0.0f;                                // 回転タイマー
	static inline const float kTimeTurn = 0.3f;             // 回転にかかる時間
	static inline const float kGravityAcceleration = 0.01f; // 重力加速度
	static inline const float kLimitFallSpeed = 0.5f;       // 落下速度の制限値
	static inline const float kJumpAcceleration = 0.3f;     // ジャンプ加速度
	MapChipField* mapChipField_ = nullptr;
	static inline const float kWidth = 0.8f;  // プレイヤーの幅
	static inline const float kHeight = 0.8f; // プレイヤーの高さ
};