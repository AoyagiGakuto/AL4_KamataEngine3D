#pragma once
#include "KamataEngine.h"
#include <memory>
#include <vector>

using namespace KamataEngine;

class DeathParticle {
public:
	struct Particle {
		WorldTransform transform;
		Vector3 velocity;

		// 残り寿命
		float lifetime;
		// 最大寿命
		float maxLifetime;
    
		// フェードアウト用カラー
		Vector4 color;
		ObjectColor objectColor;
	};

	DeathParticle();
	~DeathParticle();

	void Initialize(Model* model, Camera* camera);
	// パーティクル生成
	void Spawn(const Vector3& position);
	// 更新
	void Update();
	// 描画
	void Draw();

	// すべてのパーティクルが消えたら true
	bool IsFinished() const;

private:
	std::vector<std::unique_ptr<Particle>> particles_;
	Model* model_ = nullptr;
	Camera* camera_ = nullptr;
};
