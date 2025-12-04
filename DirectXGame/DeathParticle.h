#pragma once
#include "KamataEngine.h"
#include <memory>
#include <vector>

class DeathParticle {
public:

	struct Particle {
		KamataEngine::WorldTransform transform;
		KamataEngine::Vector3 velocity;

		/*
		// --- 寿命管理 ---
		*/

		// 残り寿命
		float lifetime;
		// 最大寿命
		float maxLifetime;

		/*
		// --- 色とフェード ---
		*/

		// フェードアウト用カラー
		KamataEngine::Vector4 color;
		KamataEngine::ObjectColor objectColor;
	};

	DeathParticle();
	~DeathParticle();

	// 初期化
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera);
	
	// パーティクル生成
	void Spawn(const KamataEngine::Vector3& position);
	
	// 更新
	void Update();
	
	// 描画
	void Draw();

	// すべてのパーティクルが消えたら true
	bool IsFinished() const;

private:

	// パーティクル群
	std::vector<std::unique_ptr<Particle>> particles_;
	
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;
};
