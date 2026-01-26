#pragma once
#include "KamataEngine.h"
#include "Player.h"

class CameraController {
public:

	// カメラ移動可能範囲
	struct Rect {
		float left = 0.0f;
		float right = 1.0;
		float bottom = 0.0f;
		float top = 1.0f;
	};

	// 初期化
	void Initialize();
	
	// 更新
	void Update();
	
	// リセット
	void Reset();

	// 追尾対象設定
	void SetTarget(Player* target) { target_ = target; };
	
	// カメラ移動可能範囲設定
	void SetMovableArea(const Rect& area) { movableArea_ = area; }

	// ビュープロジェクション行列の取得
	const KamataEngine::Camera& GetViewProjection() const { return camera_; }
	
	void Shake(float magnitude, float duration);

	// ビュー行列
	KamataEngine::Matrix4x4 matView_ = {};
	// プロジェクション行列
	KamataEngine::Matrix4x4 matProjection_ = {};

private:

	/*
	// --- カメラ ---
	*/

	// カメラ
	KamataEngine::Camera camera_;
	
	/*
	// --- 追尾対象 ---
	*/

	// 追尾対象プレイヤー
	Player* target_ = nullptr;
	// カメラの追尾オフセット
	KamataEngine::Vector3 targetOffset_ = {0, 0, -15.0f};

	/*
	// --- 制御用パラメータ ---
	*/

	// カメラの目標座標
	KamataEngine::Vector3 targetPosition_ = {0, 0, 0};
	// カメラ移動可能範囲
	Rect movableArea_ = {0, 100, 0, 100};
	
	/*
	// --- 定数パラメータ ---
	*/

	// 追従の遅延率（小さいほどゆっくり追従）
	static inline const float kInterpolationRate = 0.08f;
	// 速度の影響度合い
	static inline const float kVelocityBias = 30.0f;
	// ターゲットが画面端に寄りすぎないための余白
	static inline const Rect targetMargin = {6.0f, 100 - 12.0f, 5.0f, 5.0f};

	float shakeTimer_ = 0.0f;
	float shakeMagnitude_ = 0.0f;
};

