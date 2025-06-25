#include "DearthParticles.h"
#include <numbers>

void DearthParticles::Initialize(Model* model) {
    model_ = model;
    particles_.clear();
}

void DearthParticles::Update() {
    // パーティクルの更新
    for (auto& p : particles_) {
        p.transform.translation_.x += p.velocity.x;
        p.transform.translation_.y += p.velocity.y;
        p.transform.translation_.z += p.velocity.z;
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
        model_->Draw(p.transform, *camera_);
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
        p.transform.translation_ = position;
        p.velocity = vel;
        p.life = life;
        particles_.push_back(p);
    }
}