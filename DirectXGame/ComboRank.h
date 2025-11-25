#pragma once
#include "KamataEngine.h"
#include <array>

using namespace KamataEngine;

class ComboRank {
public:
	// コンボランク
	enum class Rank { None = 0, D, C, B, A, S, Count };

	ComboRank() = default;
	~ComboRank();

	// 初期化
	void Initialize(Camera* uiCamera, const Vector3& pos);

	// 更新
	void Update(float deltaTime);

	// 描画
	void Draw();

	// コンボ加点
	void AddHit(float value);

	// 敵撃破時ボーナス
	void OnEnemyKilled(float bonus = 0.0f);

	// プレイヤー被弾時
	void OnPlayerDamaged();

	// リセット
	void Reset();

	// 情報取得
	Rank GetRank() const { return rank_; }
	float GetPoint() const { return comboPoint_; }

private:
	void LoadModels();
	void UpdateRank();

	float comboPoint_ = 0.0f; // コンボポイント
	float decayTimer_ = 0.0f; // 最後にヒットしてからの時間
	Rank rank_ = Rank::None;

	// ランクモデル
	std::array<Model*, static_cast<size_t>(Rank::Count)> models_{};

	WorldTransform wt_;

	// UIのカメラ
	Camera* uiCamera_ = nullptr;

	// パラメータけい
	static inline const float kMaxPoint = 100.0f;
	static inline const float kStartDecayTime = 0.8f; // 開始までの猶予
	static inline const float kDecaySpeed = 15.0f;    // 1秒あたり減る量
};
