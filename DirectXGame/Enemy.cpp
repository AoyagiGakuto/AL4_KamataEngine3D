#include "Enemy.h"
#include "MyMath.h"
#include <algorithm>
#include <cassert>

using namespace KamataEngine::MathUtility;

using namespace KamataEngine;

// ==========================================================================
// 初期化処理
// ==========================================================================

void Enemy::Initialize(Model* model, Model* modelHpBar, Model* modelHp, Camera* camera, const Vector3& position, Type type) {
	assert(model);
	model_ = model;
	modelHpBar_ = modelHpBar;
	modelHp_ = modelHp;
	camera_ = camera;

	// ワールド変換の初期化
	worldTransform_.Initialize();
	worldTransformHpBar_.Initialize();
	worldTransformHp_.Initialize();

	// HPバーのスケール設定
	Vector3 hpBarScale = {0.15f, 0.15f, 0.15f};
	worldTransformHpBar_.scale_ = hpBarScale;
	worldTransformHp_.scale_ = hpBarScale;

	worldTransform_.translation_ = position;

	type_ = type;
	baseHeight_ = position.y;
	healTimer_ = 0.0f;
	walkTimer_ = 0.0f;
	slowTimer_ = 0.0f;

	// タイプ別の初期設定
	switch (type_) {
	case Type::kNormal:
		velocity_ = {-kWalkSpeed, 0.0f, 0.0f};
		hp_ = 5;
		maxHp_ = 5.0f;
		break;
	case Type::kHoming:
		velocity_ = {0.0f, 0.0f, 0.0f};
		// 追尾は少し速く
		homingMaxSpeed_ = 0.08f;
		// いまは少し弱く
		hp_ = 3;
		maxHp_ = 3.0f;
		break;
	case Type::kFlyingSupport:
		velocity_ = {0.0f, 0.0f, 0.0f};
		hp_ = 3;
		maxHp_ = 3.0f;
		break;
	}
}

// ==========================================================================
// 更新処理
// ==========================================================================

void Enemy::Update() {

	/*
	// --- ヒットストップ ---
	*/

	if (hitStopTimer_ > 0.0f) {
		hitStopTimer_ -= 1.0f / 60.0f;
		// 時間停止中は、移動もアニメーションも更新せずここで終わる
		return;
	}

	if (shotTimer_ > 0.0f) {
		shotTimer_ -= 1.0f / 60.0f;
	}

	/*
	// --- スロー処理 ---
	*/

	// 通常速度
	float speedMultiplier = 1.0f;

	if (slowTimer_ > 0.0f) {
		slowTimer_ -= 1.0f / 60.0f;
		speedMultiplier = 0.3f;
	}

	// 敵の歩行モーションのタイマーを更新
	walkTimer_ += 1.0f / 60.0f;

	/*
	// --- ノックバック処理 ---
	*/

	if (knockbackTimer_ > 0.0f) {
		knockbackTimer_ -= 1.0f / 60.0f;

		// ノックバック中は重力だけかける
		velocity_.y -= kGravityAcc;

		CollisionInfo info;
		info.move = velocity_;
		CollisionMapCheck(info);
		worldTransform_.translation_ += info.move;

		goto COMMON_UPDATE;
	}

	// ==============================================
	// 行動パターンの分岐
	// ==============================================

	switch (type_) {

		/*
		// --- 通常タイプ ---
		*/

	case Type::kNormal: {
		// 歩行アニメーション (体を揺らす)
		float param = std::sin(walkTimer_ * (std::numbers::pi_v<float> * 2.0f / kWalkMotionTime)) * (kWalkMotionAngelEnd - kWalkMotionAngelStart) + kWalkMotionAngelStart;
		float degree = kWalkMotionAngelStart + kWalkMotionAngelEnd * (param + 1.0f) / 2.0f;
		worldTransform_.rotation_.x = (degree);

		// 重力
		velocity_.y -= kGravityAcc;

		// 移動
		CollisionInfo info;
		info.move = velocity_ * speedMultiplier;
		CollisionMapCheck(info);

		// 壁に当たったら反転
		if (info.isHitWall) {
			velocity_.x *= -1.0f;
		}

		// 座標反映
		worldTransform_.translation_ += info.move;

		// 速度から向きを決める
		if (velocity_.x > 0.001f) {
			worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
		} else if (velocity_.x < -0.001f) {
			worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;
		}

		break;
	}

		/*
		// --- 追尾タイプ ---
		*/

	case Type::kHoming: {
		// 常にプレイヤーの方向を見るなどの処理も入れられる
		worldTransform_.rotation_.x = 0.0f; // 傾きなし

		if (target_) {
			float dx = target_->GetWorldTransform().translation_.x - worldTransform_.translation_.x;
			float desiredVx = 0.0f;

			// 一定距離より離れている場合のみ動く
			if (std::fabs(dx) > homingStopDist_) {
				desiredVx = (dx > 0 ? homingMaxSpeed_ : -homingMaxSpeed_);
			}

			// 加速度で補間
			float dv = desiredVx - velocity_.x;
			dv = std::clamp(dv, -homingAccel_, homingAccel_);
			velocity_.x += dv * speedMultiplier;
		}

		// 重力
		velocity_.y -= kGravityAcc;

		// 移動と当たり判定
		CollisionInfo info;
		info.move = velocity_ * speedMultiplier;
		CollisionMapCheck(info);

		worldTransform_.translation_ += info.move;

		// 向き
		if (velocity_.x > 0.001f) {
			worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
		} else if (velocity_.x < -0.001f) {
			worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;
		}

		break;
	}

		/*
		// --- 飛行回復タイプ ---
		*/

	case Type::kFlyingSupport: {
		float hoverOffset = std::sin(walkTimer_ * 2.0f) * 0.5f;

		Vector3 targetPos = worldTransform_.translation_;
		bool isHealingMode = false; // 回復モードか射撃モードか

		// 回復対象がいるかチェック
		if (healTarget_ && !healTarget_->IsDead() && healTarget_->hp_ < healTarget_->maxHp_) {
			targetPos = healTarget_->GetWorldTransform().translation_;
			isHealingMode = true;
		}
		// 射撃モードまだ未実装
		else if (target_) {
			targetPos = target_->GetWorldTransform().translation_;
			isHealingMode = false;
		}

		// 移動処理
		velocity_.x = 0.0f;
		if (isHealingMode) {
			// 回復モード
			float dx = targetPos.x - worldTransform_.translation_.x;
			float stopDist = 1.5f; // 少し離れる
			if (std::fabs(dx) > stopDist) {
				velocity_.x = (dx > 0 ? 0.02f : -0.02f);
			}
		} else {
			float dx = targetPos.x - worldTransform_.translation_.x;
			float attackSpeed = 0.05f; // 回復移動(0.02)より速くする

			if (std::fabs(dx) < 0.1f) {
				velocity_.x = 0.0f;
			} else {
				// それ以上離れていたら突撃
				velocity_.x = (dx > 0 ? attackSpeed : -attackSpeed);
			}
		}

		// 壁判定と座標更新
		CollisionInfo info;
		info.move.x = velocity_.x * speedMultiplier;
		info.move.y = 0.0f;
		CheckMapCollisionLeft(info);
		CheckMapCollisionRight(info);

		worldTransform_.translation_.x += info.move.x;
		worldTransform_.translation_.y = baseHeight_ + hoverOffset;

		// 回転制御
		if (healTimer_ > 0.0f) {
			worldTransform_.rotation_.y += 0.3f; // 回復後のクールダウン中は回転
		} else {
			// プレイヤーの方を向く
			if (target_) {
				float dx = target_->GetWorldTransform().translation_.x - worldTransform_.translation_.x;
				if (dx > 0) {
					worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
				} else {

					worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;
				}
			}
		}
		break;
	}
	}
COMMON_UPDATE:

	// ==============================================
	// HPバーの更新
	// ==============================================

	Vector3 barPos = worldTransform_.translation_;
	barPos.y += 1.5f;

	worldTransformHpBar_.translation_ = barPos;
	worldTransformHp_.translation_ = barPos;

	float hpRatio = (float)hp_ / maxHp_;
	hpRatio = std::clamp(hpRatio, 0.0f, 1.0f); // 0~1に制限

	float baseScaleX = 0.15f;
	worldTransformHp_.scale_.x = baseScaleX * hpRatio;
	worldTransformHp_.scale_.y = 0.15f;
	worldTransformHp_.scale_.z = 0.15f;

	float modelHalfWidth = 3.0f;
	float shiftAmount = (1.0f - hpRatio) * modelHalfWidth * baseScaleX;

	worldTransformHp_.translation_.x -= shiftAmount;

	worldTransformHpBar_.matWorld_ = MakeAffineMatrix(worldTransformHpBar_.scale_, worldTransformHpBar_.rotation_, worldTransformHpBar_.translation_);
	worldTransformHpBar_.TransferMatrix();

	worldTransformHp_.matWorld_ = MakeAffineMatrix(worldTransformHp_.scale_, worldTransformHp_.rotation_, worldTransformHp_.translation_);
	worldTransformHp_.TransferMatrix();

	// 自分自身の行列更新
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

// ==========================================================================
// なんちゃってAIの処理
// ==========================================================================

// 仲間(敵から見て)の回復処理
void Enemy::HealNearbyEnemies(std::list<Enemy*>& enemies) {
	if (IsDead()) {
		return;
	}

	// 傷ついている仲間を探す
	float minDist = FLT_MAX;
	Enemy* bestCandidate = nullptr;

	for (Enemy* other : enemies) {
		// 自分自身や死んでる敵は無視
		if (other == this || other->IsDead()) {
			continue;
		}

		// HPが減っているかチェック
		if (other->hp_ < other->maxHp_) {
			float dist = Length(other->GetWorldTransform().translation_ - worldTransform_.translation_);

			// 一番近い人を優先
			if (dist < minDist) {
				minDist = dist;
				bestCandidate = other;
			}
		}
	}

	// ターゲットを更新
	healTarget_ = bestCandidate;

	// クールダウン（回復後の休憩）処理
	if (healTimer_ > 0.0f) {
		healTimer_ -= 1.0f / 60.0f;
		return; // クールダウン中は回復できない
	}

	// ターゲットがいて、かつ射程圏内なら回復実行！
	if (healTarget_ && minDist < kHealRange) {
		healTarget_->TakeDamage(-2); // 2回復
		healTimer_ = kHealCooldown;  // この間クルクル回る
	}
}

// ==========================================================================
// 描画処理
// ==========================================================================

void Enemy::Draw() {
	if (model_ && camera_) {
		model_->Draw(worldTransform_, *camera_);
		if (hp_ > 0) {
			if (modelHpBar_) {
				modelHpBar_->Draw(worldTransformHpBar_, *camera_);
			}
			if (modelHp_) {

				modelHp_->Draw(worldTransformHp_, *camera_);
			}
		}
	}
}

// ==========================================================================
// 状態管理
// ==========================================================================

void Enemy::OnCollision(const Player* player) { (void)player; }

AABB Enemy::GetAABB() {
	AABB aabb;

	aabb.min = {worldTransform_.translation_.x - Enemy::kWidth / 2.0f, worldTransform_.translation_.y - Enemy::kHeight / 2.0f, worldTransform_.translation_.z - Enemy::kWidth / 2.0f};

	aabb.max = {worldTransform_.translation_.x + Enemy::kWidth / 2.0f, worldTransform_.translation_.y + Enemy::kHeight / 2.0f, worldTransform_.translation_.z + Enemy::kWidth / 2.0f};

	return aabb;
}

// ==========================================================================
// 衝突判定系
// ==========================================================================

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
	hp_ -= damage;

	// 回復した場合、最大HPを超えないようにする
	if (hp_ > (int)maxHp_) {
		hp_ = (int)maxHp_;
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

// 発射準備ができているか
bool Enemy::IsReadyToFire() {
	// 飛行タイプ以外は撃たない
	if (type_ != Type::kFlyingSupport) {
		return false;
	}

	// タイマーが0以下なら発射OK
	if (shotTimer_ <= 0.0f) {
		shotTimer_ = kFireInterval; // 次回のために時間をセット
		return true;
	}

	return false;
}

// ノックバック処理
void Enemy::Knockback(const Vector3& dir) {
	if (knockbackTimer_ > 0.0f) {
		return;
	}

	float knockbackSpeed = 0.1f;
	float jumpPower = 0.15f;

	velocity_.x = dir.x * knockbackSpeed;
	velocity_.y = jumpPower;

	knockbackTimer_ = 0.5f; // 0.5秒間吹っ飛ぶ
}

// ヒットストップ処理
void Enemy::ApplyHitStop(float duration) { hitStopTimer_ = duration; }