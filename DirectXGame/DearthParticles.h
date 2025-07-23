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
		float lifetime;
		float maxLifetime;

		// フェードアウト用
		Vector4 color;
		ObjectColor objectColor;
	};

	DeathParticle();
	~DeathParticle();

	void Initialize(Model* model, Camera* camera);
	void Spawn(const Vector3& position);
	void Update();
	void Draw();

private:
	std::vector<std::unique_ptr<Particle>> particles_;
	Model* model_ = nullptr;
	Camera* camera_ = nullptr;
};
