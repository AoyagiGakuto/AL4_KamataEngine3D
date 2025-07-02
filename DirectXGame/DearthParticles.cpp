#include "DearthParticles.h"
#include <algorithm>
#include <numbers>

void DearthParticles::Initialize(Model* model, Camera* camera, const Vector3& position) {
	assert(model);
	model_ = model;
	camera_ = camera;
	worldTransform_.translation_ = position;
	worldTransform_.Initialize();
	color_ = {1.0f, 1.0f, 1.0f, 1.0f};
	objectColor_.Initialize();
	isFinished_ = false;
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

	// wouldTransforms_をparticles_に合わせて更新
	for (size_t i = 0; i < wouldTransforms_.size(); ++i) {
		if (i < particles_.size()) {
			wouldTransforms_[i].translation_ = particles_[i].position;
			wouldTransforms_[i].transform_.translation_ = particles_[i].position;
			wouldTransforms_[i].transform_.Initialize();
		} else {
			// パーティクルが足りない場合は無効な位置にするなど
			wouldTransforms_[i].translation_ = {0, 0, 0};
			wouldTransforms_[i].transform_.translation_ = {0, 0, 0};
			wouldTransforms_[i].transform_.Initialize();
		}
	}

	color_.w = std::clamp(counter_ / kDuration, 0.0f, 1.0f);

	objectColor_.SetColor(color_);

	// 寿命が尽きたパーティクルを削除
	particles_.erase(std::remove_if(particles_.begin(), particles_.end(), [](const Particle& p) { return p.life <= 0.0f; }), particles_.end());

	counter_ += 1.0f / 60.0f;
	if (counter_ >= kDuration) {
		counter_ = kDuration;
		isFinished_ = true;
	}

	if (isFinished_) {
		return;
	}
}

void DearthParticles::Draw() {
	for (auto& worldTransform : wouldTransforms_) {
		model_->Draw(worldTransform.transform_, *camera_, &objectColor_);
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
		    std::cos(angle) * speed, std::sin(angle) * speed,
		    0.0f // 2DならZは0
		};
		Particle p;
		p.position = position;
		p.velocity = vel;
		p.life = life;
		particles_.push_back(p);
	}
}