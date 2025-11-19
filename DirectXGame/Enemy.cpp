#include "Enemy.h"
#include "MyMath.h"
#include <algorithm>
#include <cassert>

using namespace KamataEngine;

void Enemy::Initialize(Model* model, Model* modelHpBar, Model* modelHp, Camera* camera, const Vector3& position, Type type) {
	assert(model);
	model_ = model;
	modelHpBar_ = modelHpBar;
	modelHp_ = modelHp;
	camera_ = camera;
	worldTransform_.Initialize();
	worldTransformHpBar_.Initialize();
	worldTransformHp_.Initialize();
	Vector3 hpBarScale = {0.15f, 0.15f, 0.15f};
	worldTransformHpBar_.scale_ = hpBarScale;
	worldTransformHp_.scale_ = hpBarScale;
	worldTransform_.translation_ = position;
	velocity_ = {-kWalkSpeed, 0.0f, 0.0f};
	walkTimer_ = 0.0f;
	hp_ = 5;
	maxHp_ = 5.0f;
	slowTimer_ = 0.0f;
	type_ = type;
	baseHeight_ = position.y; // 出現した高さを基準に飛ぶ
	healTimer_ = 0.0f;
	
	// 飛んでるやつなら
	if (type_ == Type::kFlyingSupport) {
		velocity_ = {0.0f, 0.0f, 0.0f};
	}
}

void Enemy::Update() {

	float speedMultiplier = 1.0f; // 通常速度
	if (slowTimer_ > 0.0f) {
		slowTimer_ -= 1.0f / 60.0f;
		speedMultiplier = 0.3f;
	}

	if (type_ == Type::kNormal) {

	// 敵の歩行モーションのタイマーを更新
	walkTimer_ += 1.0f / 60.0f;
	// 歩行モーションの角度を計算
	// 回転アニメーション
	float param = std::sin(walkTimer_ * (std::numbers::pi_v<float> * 2.0f / kWalkMotionTime)) * (kWalkMotionAngelEnd - kWalkMotionAngelStart) + kWalkMotionAngelStart;

	float degree = kWalkMotionAngelStart + kWalkMotionAngelEnd * (param + 1.0f) / 2.0f; // 角度を計算

	// x軸周りの角度をラジアンに変換
	worldTransform_.rotation_.x = (degree);

	if (homing_ && target_) {
		// プレイヤーを追尾
		float dx = target_->GetWorldTransform().translation_.x - worldTransform_.translation_.x;
		float desiredVx = 0.0f;

		// 一定距離より離れている場合のみ動く
		if (std::fabs(dx) > homingStopDist_) {
			desiredVx = (dx > 0 ? homingMaxSpeed_ : -homingMaxSpeed_);
		}

		// 加速度で補間
		float dv = desiredVx - velocity_.x;
		if (dv > +homingAccel_)
			dv = +homingAccel_;
		
		if (dv < -homingAccel_)
			dv = -homingAccel_;

		velocity_.x += dv * speedMultiplier;
	}

	// 向き反転
	if (velocity_.x > 0.001f) {
		worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	} else if (velocity_.x < -0.001f) {
		worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;
	}

	// マップとの当たり判定
	CollisionInfo info;
	info.move.x = velocity_.x * speedMultiplier;
	info.move.y = velocity_.y * speedMultiplier;
	info.move.z = velocity_.z * speedMultiplier;
	CollisionMapCheck(info);

	// 衝突結果
	if (info.isHitWall && !homing_) {
		// ホーミングしていない敵が壁に当たったら反転
		velocity_.x *= -1.0f;
	}

	worldTransform_.translation_.x += info.move.x;
	worldTransform_.translation_.y += info.move.y;
	worldTransform_.translation_.z += info.move.z;

	// HPP

	// 敵の頭上に配置 (敵の座標 + Y方向に少し上)
	Vector3 barPos = worldTransform_.translation_;
	barPos.y += 1.5f; // 敵の高さ

	worldTransformHpBar_.translation_ = barPos;
	worldTransformHp_.translation_ = barPos;

	// HPの割合に合わせてバーを縮める
	float hpRatio = (float)hp_ / maxHp_;

	// 0以下にならないように制限
	if (hpRatio < 0.0f) {
		hpRatio = 0.0f;
	}

	float baseScaleX = 0.15f;

	// スケールを変える
	worldTransformHp_.scale_.x = baseScaleX * hpRatio;
	worldTransformHp_.scale_.y = 0.15f;
	worldTransformHp_.scale_.z = 0.15f;

	float modelHalfWidth = 3.0f;
	float shiftAmount = (1.0f - hpRatio) * modelHalfWidth * baseScaleX;

	worldTransformHp_.translation_ = barPos;
	worldTransformHp_.translation_.x -= shiftAmount;

	worldTransformHpBar_.matWorld_ = MakeAffineMatrix(worldTransformHpBar_.scale_, worldTransformHpBar_.rotation_, worldTransformHpBar_.translation_);
	worldTransformHpBar_.TransferMatrix();

	worldTransformHp_.matWorld_ = MakeAffineMatrix(worldTransformHp_.scale_, worldTransformHp_.rotation_, worldTransformHp_.translation_);
	worldTransformHp_.TransferMatrix();

	// 行列更新
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

void Enemy::Draw() {
	if (model_ && camera_) {
		model_->Draw(worldTransform_, *camera_);
		if (hp_ > 0) {
			// 枠を描画
			if (modelHpBar_)
				modelHpBar_->Draw(worldTransformHpBar_, *camera_);
			// 中身を描画
			if (modelHp_)
				modelHp_->Draw(worldTransformHp_, *camera_);
		}
	}
}

void Enemy::OnCollision(const Player* player) { (void)player; }

AABB Enemy::GetAABB() {
	AABB aabb;

	aabb.min = {worldTransform_.translation_.x - Enemy::kWidth / 2.0f, worldTransform_.translation_.y - Enemy::kHeight / 2.0f, worldTransform_.translation_.z - Enemy::kWidth / 2.0f};

	aabb.max = {worldTransform_.translation_.x + Enemy::kWidth / 2.0f, worldTransform_.translation_.y + Enemy::kHeight / 2.0f, worldTransform_.translation_.z + Enemy::kWidth / 2.0f};

	return aabb;
}

void Enemy::CollisionMapCheck(CollisionInfo& info) {
	CheckMapCollisionDown(info);
	CheckMapCollisionUp(info);
	CheckMapCollisionLeft(info);
	CheckMapCollisionRight(info);
}

void Enemy::CheckMapCollisionDown(CollisionInfo& info) {
	if (info.move.y >= 0.0f) {
		return;
	}

	assert(mapChipField_);

	float foot = worldTransform_.translation_.y - kHeight / 2.0f;
	float newFoot = foot + info.move.y;
	float playerLeft = worldTransform_.translation_.x - kWidth / 2.0f;
	float playerRight = worldTransform_.translation_.x + kWidth / 2.0f;

	// 足元の左右2点をチェック
	MapChipField::IndexSet indexSet_L = mapChipField_->GetMapChipIndexSetByPosition({playerLeft, newFoot, 0.0f});
	MapChipField::IndexSet indexSet_R = mapChipField_->GetMapChipIndexSetByPosition({playerRight - kBlank, newFoot, 0.0f});

	MapChipType type_L = mapChipField_->GetMapChipTypeByIndex(indexSet_L.xIndex, indexSet_L.yIndex);
	MapChipType type_R = mapChipField_->GetMapChipTypeByIndex(indexSet_R.xIndex, indexSet_R.yIndex);

	if (type_L == MapChipType::kBlock || type_R == MapChipType::kBlock) {
		// -FLT_MAXはfloatの最小値0で初期化するとバグが起きちゃうかもしれない
		// まちがって0で初期化しないように注意
		float blockTop = -FLT_MAX;
		if (type_L == MapChipType::kBlock) {
			float topL = mapChipField_->GetRectByIndex(indexSet_L.xIndex, indexSet_L.yIndex).top;
			blockTop = (topL > blockTop) ? topL : blockTop;
		}
		if (type_R == MapChipType::kBlock) {
			float topR = mapChipField_->GetRectByIndex(indexSet_R.xIndex, indexSet_R.yIndex).top;
			blockTop = (topR > blockTop) ? topR : blockTop;
		}

		worldTransform_.translation_.y = blockTop + kHeight / 2.0f;

		info.move.y = 0.0f;
		velocity_.y = 0.0f;
		info.isOnGround = true;
	}
}

void Enemy::CheckMapCollisionUp(CollisionInfo& info) {
	if (info.move.y <= 0.0f) {
		return;
	}

	assert(mapChipField_);

	float head = worldTransform_.translation_.y + kHeight / 2.0f;
	float newHead = head + info.move.y;
	float playerLeft = worldTransform_.translation_.x - kWidth / 2.0f;
	float playerRight = worldTransform_.translation_.x + kWidth / 2.0f;

	// 上の左右2点をチェック
	MapChipField::IndexSet indexSet_L = mapChipField_->GetMapChipIndexSetByPosition({playerLeft, newHead, 0.0f});
	MapChipField::IndexSet indexSet_R = mapChipField_->GetMapChipIndexSetByPosition({playerRight - kBlank, newHead, 0.0f});

	MapChipType type_L = mapChipField_->GetMapChipTypeByIndex(indexSet_L.xIndex, indexSet_L.yIndex);
	MapChipType type_R = mapChipField_->GetMapChipTypeByIndex(indexSet_R.xIndex, indexSet_R.yIndex);

	if (type_L == MapChipType::kBlock || type_R == MapChipType::kBlock) {
		float blockBottom = FLT_MAX;
		if (type_L == MapChipType::kBlock) {
			float bottomL = mapChipField_->GetRectByIndex(indexSet_L.xIndex, indexSet_L.yIndex).bottom;
			blockBottom = (bottomL < blockBottom) ? bottomL : blockBottom;
		}
		if (type_R == MapChipType::kBlock) {
			float bottomR = mapChipField_->GetRectByIndex(indexSet_R.xIndex, indexSet_R.yIndex).bottom;
			blockBottom = (bottomR < blockBottom) ? bottomR : blockBottom;
		}

		worldTransform_.translation_.y = blockBottom - kHeight / 2.0f;

		info.move.y = 0.0f;
		velocity_.y = 0.0f;
		info.isCeiling = true;
	}
}

void Enemy::CheckMapCollisionLeft(CollisionInfo& info) {
	if (info.move.x >= 0.0f) {
		return;
	}

	assert(mapChipField_);

	float left = worldTransform_.translation_.x - kWidth / 2.0f;
	float newLeft = left + info.move.x;
	float playerTop = worldTransform_.translation_.y + kHeight / 2.0f;
	float playerBottom = worldTransform_.translation_.y - kHeight / 2.0f;

	// 左側の上下2点をチェック
	MapChipField::IndexSet indexSet_T = mapChipField_->GetMapChipIndexSetByPosition({newLeft, playerTop - kBlank, 0.0f});
	MapChipField::IndexSet indexSet_B = mapChipField_->GetMapChipIndexSetByPosition({newLeft, playerBottom, 0.0f});

	MapChipType type_T = mapChipField_->GetMapChipTypeByIndex(indexSet_T.xIndex, indexSet_T.yIndex);
	MapChipType type_B = mapChipField_->GetMapChipTypeByIndex(indexSet_B.xIndex, indexSet_B.yIndex);

	if (type_T == MapChipType::kBlock || type_B == MapChipType::kBlock) {
		float blockRight = -FLT_MAX;
		if (type_T == MapChipType::kBlock) {
			float rightT = mapChipField_->GetRectByIndex(indexSet_T.xIndex, indexSet_T.yIndex).right;
			blockRight = (rightT > blockRight) ? rightT : blockRight;
		}
		if (type_B == MapChipType::kBlock) {
			float rightB = mapChipField_->GetRectByIndex(indexSet_B.xIndex, indexSet_B.yIndex).right;
			blockRight = (rightB > blockRight) ? rightB : blockRight;
		}

		worldTransform_.translation_.x = blockRight + kWidth / 2.0f;

		info.move.x = 0.0f;
		info.isHitWall = true;
	}
}

void Enemy::CheckMapCollisionRight(CollisionInfo& info) {
	if (info.move.x <= 0.0f) {
		return;
	}

	assert(mapChipField_);

	float right = worldTransform_.translation_.x + kWidth / 2.0f;
	float newRight = right + info.move.x;
	float playerTop = worldTransform_.translation_.y + kHeight / 2.0f;
	float playerBottom = worldTransform_.translation_.y - kHeight / 2.0f;

	// 右側の上下2点をチェック
	MapChipField::IndexSet indexSet_T = mapChipField_->GetMapChipIndexSetByPosition({newRight, playerTop - kBlank, 0.0f});
	MapChipField::IndexSet indexSet_B = mapChipField_->GetMapChipIndexSetByPosition({newRight, playerBottom, 0.0f});

	MapChipType type_T = mapChipField_->GetMapChipTypeByIndex(indexSet_T.xIndex, indexSet_T.yIndex);
	MapChipType type_B = mapChipField_->GetMapChipTypeByIndex(indexSet_B.xIndex, indexSet_B.yIndex);

	if (type_T == MapChipType::kBlock || type_B == MapChipType::kBlock) {
		float blockLeft = FLT_MAX;
		if (type_T == MapChipType::kBlock) {
			float leftT = mapChipField_->GetRectByIndex(indexSet_T.xIndex, indexSet_T.yIndex).left;
			blockLeft = (leftT < blockLeft) ? leftT : blockLeft;
		}
		if (type_B == MapChipType::kBlock) {
			float leftB = mapChipField_->GetRectByIndex(indexSet_B.xIndex, indexSet_B.yIndex).left;
			blockLeft = (leftB < blockLeft) ? leftB : blockLeft;
		}

		worldTransform_.translation_.x = blockLeft - kWidth / 2.0f;

		info.move.x = 0.0f;
		info.isHitWall = true;
	}
}

// ダメージを受ける
void Enemy::TakeDamage(int damage) {
	if (hp_ > 0) {
		hp_ -= damage;
	}
}

// 死亡しているか
bool Enemy::IsDead() const { return hp_ <= 0; }

// 動きをスローにする
void Enemy::SlowDown(float duration) {
	if (slowTimer_ < duration) {
		slowTimer_ = duration;
	}
}