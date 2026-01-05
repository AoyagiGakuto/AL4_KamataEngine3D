#include "DeathParticle.h"
#include "MyMath.h"
#include <algorithm>
#include "GameScene.h"

using namespace KamataEngine;

DeathParticle::DeathParticle() {}
DeathParticle::~DeathParticle() {}

// ==========================================================================
// 初期化
// ==========================================================================

void DeathParticle::Initialize(Model* model, Camera* camera) {
	model_ = model;
	camera_ = camera;
}

// ==========================================================================
// 生成処理
// ==========================================================================

void DeathParticle::Spawn(const Vector3& position) {
	// XY平面の8方向
	Vector3 directions[8] = {
	    {0.0f,  1.0f,  0.0f}, // 上
	    {0.7f,  0.7f,  0.0f}, // 右上
	    {1.0f,  0.0f,  0.0f}, // 右
	    {0.7f,  -0.7f, 0.0f}, // 右下
	    {0.0f,  -1.0f, 0.0f}, // 下
	    {-0.7f, -0.7f, 0.0f}, // 左下
	    {-1.0f, 0.0f,  0.0f}, // 左
	    {-0.7f, 0.7f,  0.0f}  // 左上
	};

	// ゆっくり飛ばす
	float baseSpeed = 0.15f;

	for (int i = 0; i < 8; i++) {
		auto p = std::make_unique<Particle>();

		// 初期化
		p->transform.Initialize();
		p->transform.translation_ = position;

		// 速度設定
		Vector3 dir = directions[i];
		p->velocity = {dir.x * baseSpeed, dir.y * baseSpeed, dir.z * baseSpeed};

		// サイズはプレイヤーくらい
		p->transform.scale_ = {1.0f, 1.0f, 1.0f};

		// 寿命設定
		p->lifetime = 1.5f;
		p->maxLifetime = 1.5f;

		// カラー初期化
		p->color = {1.0f, 1.0f, 1.0f, 1.0f};
		p->objectColor.Initialize();
		p->objectColor.SetColor(p->color);

		particles_.push_back(std::move(p));
	}
}

// ==========================================================================
// 更新処理
// ==========================================================================

void DeathParticle::Update() {
	for (auto& p : particles_) {
		// 位置更新
		p->transform.translation_.x += p->velocity.x;
		p->transform.translation_.y += p->velocity.y;
		p->transform.translation_.z += p->velocity.z;

		// 減速
		p->velocity.x *= 0.98f;
		p->velocity.y *= 0.98f;
		p->velocity.z *= 0.98f;

		// 寿命減少
		p->lifetime -= 1.0f / 60.0f;

		// 残り寿命比率（0.0～1.0）
		float lifeRatio = p->lifetime / p->maxLifetime;
		float alpha = std::clamp(lifeRatio, 0.0f, 1.0f);

		// フェードアウト（α値を減らす）
		p->color.w = alpha;
		p->objectColor.SetColor(p->color);

		// 行列更新
		p->transform.matWorld_ = MakeAffineMatrix(p->transform.scale_, p->transform.rotation_, p->transform.translation_);
		p->transform.TransferMatrix();
	}

	// 寿命切れのパーティクル削除
	particles_.erase(std::remove_if(particles_.begin(), particles_.end(), [](const std::unique_ptr<Particle>& p) { return p->lifetime <= 0.0f; }), particles_.end());
}

// ==========================================================================
// 描画処理
// ==========================================================================

void DeathParticle::Draw() {
	if (!model_ || !camera_) {
		return;
	}

	for (auto& p : particles_) {
		model_->Draw(p->transform, *camera_, &p->objectColor);
	}
}

// ==========================================================================
// 判定
// ==========================================================================

bool DeathParticle::IsFinished() const {
	// パーティクルが全部消えたら true
	return particles_.empty();
}
