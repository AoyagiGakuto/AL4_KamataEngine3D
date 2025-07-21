#pragma once
#include "MapChipField.h"
#include "MyMath.h"
#include "Player.h"
#include <algorithm>
#include <array>
#include <numbers>
#include <vector>

using namespace KamataEngine;

struct Particle {
	Vector3 position;
	Vector3 velocity;
	float life;
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
	ObjectColor objectColor_;
	Vector4 color_;
	WorldTransform worldTransform_;
	Vector4 whiteColor = {1.0f, 1.0f, 1.0f, 1.0f};
	std::vector<Particle> particles_;
	Camera* camera_ = nullptr;
	Model* model_ = nullptr;
	static inline const uint32_t kNumParticles = 8;
	std::array<WouldTransform, kNumParticles> wouldTransforms_;
	bool isFinished_ = false;                   // パーティクルが終了したかどうか
	float counter_ = 0.0f;                      // パーティクルのカウンター
	static inline const float kDuration = 1.0f; // パーティクルの寿命
	static inline const float kspeed = 0.1f;    // パーティクルの速度
	static inline const float kAngleUnit =      // 2π/ 分割数　
	    std::numbers::pi_v<float> / 4.0f;       // 45度ずつの角度
};