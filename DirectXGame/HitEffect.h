#pragma once
#include "KamataEngine.h"
#include "MyMath.h"
#include <array>
#include <string>
#include <vector>

class HitEffect {
public:

	~HitEffect();

	// 初期化
	void Initialize(std::array<KamataEngine::Model*, 10>& numberModels, KamataEngine::Camera* camera, const KamataEngine::Vector3& pos, int number);

	// 更新
	void Update();
	
	// 描画
	void Draw();

	// 位置更新
	void UpdatePosition(const KamataEngine::Vector3& basePos);

	// 生存確認
	bool IsAlive() const { return alive_; }

private:

	// ワールド変換群
	std::vector<KamataEngine::WorldTransform*> transforms_;

	// モデル群
	std::vector<KamataEngine::Model*> modelsToDraw_;

	KamataEngine::Camera* camera_ = nullptr;
	KamataEngine::ObjectColor objectColor_; // フェード用の色

	/*
	// --- 状態 ---
	*/

	bool alive_ = true;
	float lifetime_ = 0.75f;
	// 上昇量
	float yOffset_ = 0.0f;
	// 表示する数字文字列
	std::string numStr_ = "";
	KamataEngine::Vector3 basePos_ = {};

	/*
	// --- パラメータ ---
	*/

	float maxLifetime_ = 0.75f;
	// 上昇速度
	float speed_ = 0.02f;
	// 文字サイズ
	float scale_ = 0.5f;

	KamataEngine::Vector4 color_ = {1.0f, 1.0f, 1.0f, 1.0f};

	// 文字の間隔
	static inline const float kDigitWidth = 0.4f;
};