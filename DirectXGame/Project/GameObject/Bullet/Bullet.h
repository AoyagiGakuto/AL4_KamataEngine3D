#pragma once
#include "KamataEngine.h"
#include "MyMath.h"

class Bullet {
public:

	// 弾の種類
	enum class Type {
		kPlayer, // プレイヤーの弾
		kEnemy   // 敵の弾
	};

	// 初期化
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& pos, const KamataEngine::Vector3& dir);

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

	// 当たり判定用
	void Kill() { alive_ = false; }

	// 弾の種類設定
	void SetType(Type type) { type_ = type; }

	// 生存確認フラグ
	bool IsAlive() const { return alive_; }

	// 当たり判定用AABB取得
	AABB GetAABB() const;

	// 弾の種類取得
	Type GetType() const { return type_; }

private:

	/*
	// --- 移動系 ---
	*/

	// ワールド変換情報
	KamataEngine::WorldTransform transform_;
	// 進行方向
	KamataEngine::Vector3 direction_ = {1.0f, 0.0f, 0.0f};
	// 速度
	float speed_ = 0.25f;

	/*
	// --- 生存管理 ---
	*/

	// 生存時間
	float lifetime_ = 2.0f;
	// 生存フラグ
	bool alive_ = true;
	// 死亡フラグ（当たり判定用）
	bool isDead_ = false;

	/*
	// --- 描画系 ---
	*/

	// モデル
	KamataEngine::Model* model_ = nullptr;
	// カメラ
	KamataEngine::Camera* camera_ = nullptr;
	// 弾の種類
	Type type_ = Type::kPlayer;
};
