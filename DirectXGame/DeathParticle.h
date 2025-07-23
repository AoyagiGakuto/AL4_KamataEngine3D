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
		float lifetime;    // 残り寿命
		float maxLifetime; // 最大寿命

		// フェードアウト用カラー
		Vector4 color;
		ObjectColor objectColor;
	};

	DeathParticle();
	~DeathParticle();

	void Initialize(Model* model, Camera* camera);
	void Spawn(const Vector3& position); // パーティクル生成
	void Update();                       // 更新
	void Draw();                         // 描画

	// すべてのパーティクルが消えたら true
	bool IsFinished() const;

private:
	std::vector<std::unique_ptr<Particle>> particles_;
	Model* model_ = nullptr;
	Camera* camera_ = nullptr;
};
