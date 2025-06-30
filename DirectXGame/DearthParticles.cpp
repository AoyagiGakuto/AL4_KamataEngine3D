#include "DearthParticles.h"
#include <numbers>

void DearthParticles::Initialize(Model* model, Camera* camera, const Vector3& position) {
	assert(model);
    model_ = model;
	camera_ = camera;
	for (WouldTransform& wouldTransform : wouldTransforms_) {
		wouldTransform.initialize();
		worldTransform_.translation_ = position;
    }
}

void DearthParticles::Update() {
    // パーティクルの更新
    for (auto& p : particles_) {
		worldTransform_.translation_.x += p.velocity.x;
		worldTransform_.translation_.y += p.velocity.y;
		worldTransform_.translation_.z += p.velocity.z;
        p.life -= 1.0f / 60.0f; // 60FPS想定
    }
    // 寿命が尽きたパーティクルを削除
    particles_.erase(
        std::remove_if(particles_.begin(), particles_.end(),
            [](const Particle& p) { return p.life <= 0.0f; }),
        particles_.end()
    );
}

void DearthParticles::Draw() {  
    if (!model_) return;  
    for (const auto& p : particles_) {  
        // パーティクルの位置を反映して描画  
        WorldTransform particleTransform;  
        particleTransform.scale_ = worldTransform_.scale_;  
        particleTransform.rotation_ = worldTransform_.rotation_;  
        particleTransform.translation_ = worldTransform_.translation_;  
        particleTransform.translation_.x += p.velocity.x;  
        particleTransform.translation_.y += p.velocity.y;  
        particleTransform.translation_.z += p.velocity.z;  
        model_->Draw(particleTransform, *camera_);  
    }  
}

void DearthParticles::Emit8Directions(const Vector3& position, float speed, float life) {
    using namespace std::numbers;
    for (int i = 0; i < 8; ++i) {
        float angle = i * (pi_v<float> / 4.0f); // 45度ずつ
        Vector3 vel = {
            std::cos(angle) * speed,
            std::sin(angle) * speed,
            0.0f // 2DならZは0
        };
        Particle p;
		worldTransform_.translation_ = position;
        p.velocity = vel;
        p.life = life;
        particles_.push_back(p);
    }
}