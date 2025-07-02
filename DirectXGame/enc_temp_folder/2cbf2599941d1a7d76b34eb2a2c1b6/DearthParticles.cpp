#include "DearthParticles.h"
#include <algorithm> // std::remove_if, std::clamp
#include <numbers>

DearthParticles::DearthParticles(Model* model) {
	assert(model);
	model_ = model;
}

void DearthParticles::Initialize(Camera* camera, const Vector3& position) {
	camera_ = camera;
	worldTransform_.translation_ = position;
	worldTransform_.Initialize();
	objectColor_.Initialize();
	color_ = {1.0f, 1.0f, 1.0f, 1.0f};
	isFinished_ = false;
	objectColor_.Initialize();
	counter_ = 0.0f;
	particles_.clear();
}

void DearthParticles::Update() {
	// パーティクルの更新
	for (auto& p : particles_) {
		p.position.x += p.velocity.x;
		p.position.y += p.velocity.y;
		p.position.z += p.velocity.z;
		p.life -= 1.0f / 60.0f; // 60FPS想定
	}
	// 寿命が尽きたパーティクルを削除
	particles_.erase(
		std::remove_if(particles_.begin(), particles_.end(),
			[](const Particle& p) { return p.life <= 0.0f; }),
		particles_.end()
	);

	counter_ += 1.0f / 60.0f;
	if (counter_ >= kDuration) {
		counter_ = kDuration;
		isFinished_ = true;
	}

	color_.w = std::clamp(color_.w - 0.01f, 0.0f, 1.0f); // 色の透明度を減少
	objectColor_.SetColor(color_);

	if (isFinished_) {
		return;
	}
}

void DearthParticles::Draw() {
	if (!model_ || !camera_)
		return;
	for (const auto& p : particles_) {
		WorldTransform wt;
		wt.translation_ = p.position;
		wt.Initialize();
		model_->Draw(wt, *camera_, &objectColor_);
	}
}

void DearthParticles::Emit8Directions(const Vector3& position, float speed, float life) {
	using namespace std::numbers;
	particles_.clear();
	isFinished_ = false;
	counter_ = 0.0f;
	for (int i = 0; i < 8; ++i) {
		float angle = i * (pi_v<float> / 4.0f); // 45度ずつ
		Vector3 vel = {
			std::cos(angle) * speed,
			std::sin(angle) * speed,
			0.0f // 2DならZは0
		};
		Particle p;
		p.position = position;
		p.velocity = vel;
		p.life = life;
		particles_.push_back(p);
	}
}