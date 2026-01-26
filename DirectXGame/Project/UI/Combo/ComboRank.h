#pragma once
#include "KamataEngine.h"
#include <array>

class ComboRank {
public:

	// コンボランク
	enum class Rank { None = 0, D, C, B, A, S, Count };

	ComboRank() = default;
	~ComboRank();

	// 初期化
	void Initialize(KamataEngine::Camera* uiCamera, const KamataEngine::Vector3& pos, KamataEngine::Model* modelBar, KamataEngine::Model* modelGauge);

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

	// ゲッター
	Rank GetRank() const { return rank_; }
	float GetPoint() const { return comboPoint_; }

private:

	void LoadModels();
	void UpdateRank();

	/*
	// --- 状態データ ---
	*/

	// コンボポイント
	float comboPoint_ = 0.0f;
	// 最後にヒットしてからの時間
	float decayTimer_ = 0.0f;
	// ランク
	Rank rank_ = Rank::None;

	/*
	// --- 描画系 ---
	*/

	// ランクモデル
	std::array<KamataEngine::Model*, static_cast<size_t>(Rank::Count)> models_{};

	// ワールド変換
	KamataEngine::WorldTransform wt_;

	// UIのカメラ
	KamataEngine::Camera* uiCamera_ = nullptr;

	KamataEngine::Model* modelBar_ = nullptr;   // 枠
	KamataEngine::Model* modelGauge_ = nullptr; // 中身
	KamataEngine::WorldTransform wtBar_;
	KamataEngine::WorldTransform wtGauge_;

	// パラメータ
	static inline const float kMaxPoint = 100.0f;
	// 開始までの猶予
	static inline const float kStartDecayTime = 0.8f;
	// 1秒あたり減る量
	static inline const float kDecaySpeed = 15.0f;

	// ゲージの表示調整
	static inline const KamataEngine::Vector3 kGaugeScale = {0.5f, 0.1f, 0.1f}; // 少し小さめに
	static inline const float kGaugeOffsetY = -0.8f;                            // ランク文字の下に配置
};
