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

	void SetRotation(float playerRotationY);

	// ランダムな回転をセット
	void SetRandomRotation();

private:
	KamataEngine::WorldTransform transform_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;
	KamataEngine::ObjectColor objectColor_;
	KamataEngine::Vector4 color_ = {1.0f, 1.0f, 1.0f, 1.0f};

	// 0.4秒で消える
	float lifetime_ = 0.4f;
	float maxLifetime_ = 0.4f;
	bool alive_ = true;
};