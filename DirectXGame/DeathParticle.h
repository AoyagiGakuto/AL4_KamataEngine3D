#pragma once
#include "KamataEngine.h"
#include <memory>
#include <vector>

class DeathParticle {
public:

	struct Particle {
		KamataEngine::WorldTransform transform;
		KamataEngine::Vector3 velocity;
		// 残り寿命
		float lifetime;
		// 最大寿命
		float maxLifetime;

		// フェードアウト用カラー
		KamataEngine::Vector4 color;
		KamataEngine::ObjectColor objectColor;
	};

	DeathParticle();
	~DeathParticle();

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
	std::vector<std::unique_ptr<Particle>> particles_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;
};
