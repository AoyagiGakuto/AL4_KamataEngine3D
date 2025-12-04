#pragma once
#include "KamataEngine.h"
#include "MyMath.h"

class SlashEffect {
public:
	~SlashEffect();

	// 初期化
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& pos);

	// 更新
	void Update();

	// 描画
	void Draw();

	// 生存確認
	bool IsAlive() const { return alive_; }

	// プレイヤーの向きに合わせて回転をセット
	void SetRotation(float playerRotationY);

	// ランダムな回転をセット
	void SetRandomRotation();

private:

	/*
	// --- 描画関係 ---
	*/

	KamataEngine::WorldTransform transform_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;

	/*
	// --- 色 ---
	*/

	KamataEngine::ObjectColor objectColor_;
	KamataEngine::Vector4 color_ = {1.0f, 1.0f, 1.0f, 1.0f};

	/*
	// --- 寿命管理 ---
	*/

	// 残り寿命（0.4秒で消える）
	float lifetime_ = 0.4f;
	// 最大寿命
	float maxLifetime_ = 0.4f;
	bool alive_ = true;
};