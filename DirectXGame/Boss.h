#pragma once
#include "KamataEngine.h"
#include "MyMath.h"
#include "Player.h"

/*
using namespace KamataEngine;

class Boss {
public:
	void Initialize(Model* model, Camera* camera, const Vector3& pos) {
		model_ = model;
		camera_ = camera;
		wt_.Initialize();
		wt_.translation_ = pos;
		wt_.scale_ = {0.8f, 0.8f, 0.8f}; // 大きめ表示
		hp_ = maxHP_;
	}

	void Update(const Player* player) {
		const float dt = 1.0f / 60.0f;
		// 追尾向きだけ合わせる（+Xが正面のルール）
		if (player) {
			if (player->GetWorldTransform().translation_.x > wt_.translation_.x) {
				wt_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
				faceRight_ = true;
			} else {
				wt_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;
				faceRight_ = false;
			}
			targetX_ = player->GetWorldTransform().translation_.x;
		}

		timer_ -= dt;

		switch (state_) {
		case State::Idle:
			vx_ *= 0.90f; // ちょい減速
			if (timer_ <= 0.0f) {
				// プレイヤーと一定距離ならダッシュ準備
				if (std::fabs(targetX_ - wt_.translation_.x) < 6.0f) {
					state_ = State::Windup;
					timer_ = 0.40f; // 溜め
				} else {
					timer_ = 0.30f; // しばらく様子見
				}
			}
			break;

		case State::Windup:
			vx_ *= 0.85f; // 止まる
			if (timer_ <= 0.0f) {
				state_ = State::Dash;
				timer_ = 0.25f; // ダッシュ時間
				vx_ = (faceRight_ ? +dashSpeed_ : -dashSpeed_);
			}
			break;

		case State::Dash:
			// 直進。時間で終わる
			if (timer_ <= 0.0f) {
				state_ = State::Recover;
				timer_ = 0.35f; // 硬直
			}
			break;

		case State::Recover:
			vx_ *= 0.90f;
			if (timer_ <= 0.0f) {
				state_ = State::Idle;
				timer_ = 0.20f;
			}
			break;

		case State::Stagger:
			vx_ *= 0.80f;
			if (timer_ <= 0.0f) {
				state_ = State::Idle;
				timer_ = 0.30f;
			}
			break;
		}

		// 位置更新（横だけ）
		wt_.translation_.x += vx_;
		// 行列更新
		wt_.matWorld_ = MakeAffineMatrix(wt_.scale_, wt_.rotation_, wt_.translation_);
		wt_.TransferMatrix();
	}

	void Draw() {
		if (model_ && camera_) {
			model_->Draw(wt_, *camera_);
		}
	}

	// --- 当たり判定/HP ---
	AABB GetAABB() const {
		// ボスは大きめに
		const float w = 1.2f, h = 1.2f;
		AABB a;
		a.min = {wt_.translation_.x - w * 0.5f, wt_.translation_.y - h * 0.5f, wt_.translation_.z - w * 0.5f};
		a.max = {wt_.translation_.x + w * 0.5f, wt_.translation_.y + h * 0.5f, wt_.translation_.z + w * 0.5f};
		return a;
	}

	bool IsDashing() const { return state_ == State::Dash; }

	void TakeDamage(int d) {
		if (hp_ <= 0)
			return;
		hp_ -= d;
		if (hp_ <= 0) {
			dead_ = true;
		} else {
			// のけぞり
			state_ = State::Stagger;
			timer_ = 0.20f;
			vx_ = (faceRight_ ? -knockback_ : +knockback_);
		}
	}

	bool IsDead() const { return dead_; }
	const WorldTransform& GetWorldTransform() const { return wt_; }
	int GetHP() const { return hp_; }
	int GetMaxHP() const { return maxHP_; }

private:
	enum class State { Idle, Windup, Dash, Recover, Stagger };

	WorldTransform wt_;
	Model* model_ = nullptr;
	Camera* camera_ = nullptr;

	// 状態
	State state_ = State::Idle;
	float timer_ = 0.0f;
	float vx_ = 0.0f;
	bool faceRight_ = true;
	float targetX_ = 0.0f;

	// パラメータ
	const int maxHP_ = 200;
	int hp_ = 200;
	const float dashSpeed_ = 0.25f;
	const float knockback_ = 0.12f;

	bool dead_ = false;
};

*/