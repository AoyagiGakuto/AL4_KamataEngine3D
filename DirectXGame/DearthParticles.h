#pragma once
#include "MapChipField.h"
#include "MyMath.h"
#include "Player.h"
#include <algorithm>
#include <numbers>
#include <vector>
#include <array>

using namespace KamataEngine;

struct Particle {
	Vector3 velocity;
	float life; // 残り寿命
};

struct WouldTransform {
	WorldTransform transform_;
	Vector3 translation_;

	void initialize() {
		transform_.Initialize();
		translation_ = {0, 0, 0};
	}
};

class DearthParticles {
public:
	void Initialize(Model* model, Camera* camera, const Vector3& position);
	void Update();
	void Draw();

	void Emit8Directions(const KamataEngine::Vector3& position, float speed, float life);

	void SetCamera(KamataEngine::Camera* camera) { camera_ = camera; }

private:
	WorldTransform worldTransform_;
	std::vector<Particle> particles_;
	KamataEngine::Camera* camera_ = nullptr;
	KamataEngine::Model* model_ = nullptr;
	static inline const uint32_t kNumParticles = 8;
	std::array<WouldTransform, kNumParticles> wouldTransforms_;
};
