#include "DeathParticle.h"
#include "MyMath.h"
#include <algorithm> // std::clamp

DeathParticle::DeathParticle() {}
DeathParticle::~DeathParticle() {}

void DeathParticle::Initialize(Model* model, Camera* camera) {
	model_ = model;
	camera_ = camera;
}

void DeathParticle::Spawn(const Vector3& position) {
	// XY平面の8方向（上下左右＋斜め）
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

	float baseSpeed = 0.15f; // ゆっくり飛ばす

	for (int i = 0; i < 8; i++) {
		auto p = std::make_unique<Particle>();
		p->transform.Initialize();
		p->transform.translation_ = position;

		Vector3 dir = directions[i];
		p->velocity = {dir.x * baseSpeed, dir.y * baseSpeed, dir.z * baseSpeed};

		// プレイヤーくらいのサイズ
		p->transform.scale_ = {1.0f, 1.0f, 1.0f};

		// 寿命設定
		p->lifetime = 1.5f;
		p->maxLifetime = 1.5f;

		// カラー初期化（完全不透明）
		p->color = {1.0f, 1.0f, 1.0f, 1.0f};
		p->objectColor.Initialize();
		p->objectColor.SetColor(p->color);

		particles_.push_back(std::move(p));
	}
}


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

		// 残り寿命比率でα値を計算
		float lifeRatio = p->lifetime / p->maxLifetime;
		float alpha = std::clamp(lifeRatio, 0.0f, 1.0f);

		// カラーのアルファ値だけ減らす
		p->color.w = alpha;
		p->objectColor.SetColor(p->color);

		// 行列更新
		p->transform.matWorld_ = MakeAffineMatrix(p->transform.scale_, p->transform.rotation_, p->transform.translation_);
		p->transform.TransferMatrix();
	}

	// 寿命切れ削除
	particles_.erase(std::remove_if(particles_.begin(), particles_.end(), [](const std::unique_ptr<Particle>& p) { return p->lifetime <= 0.0f; }), particles_.end());
}

void DeathParticle::Draw() {
	if (!model_ || !camera_)
		return;

	for (auto& p : particles_) {
		// 各パーティクルごとの objectColor を渡して描画
		model_->Draw(p->transform, *camera_, &p->objectColor);
	}
}
