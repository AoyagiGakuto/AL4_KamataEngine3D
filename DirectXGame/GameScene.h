#pragma once
#include "Bullet.h"
#include "CameraController.h"
#include "ComboRank.h"
#include "DeathParticle.h"
#include "Enemy.h"
#include "Fade.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Player.h"
#include "ScenePhase.h"
#include "Skydome.h"
#include "SlashEffect.h"

#include <array>
#include <memory>
#include <vector>

class GameScene {
public:
	~GameScene();

	// 初期化
	void Initialize();
	// 更新処理
	void Update();
	// 描画処理
	void Draw();

	// シーン終了確認
	bool IsFinished() const { return finished_; }

private:

	/*
	// --- 定数パラメータ定義 ---
	*/

	struct GameParam {
		
		/*
		// --- UI配置 ---
		*/

		static inline const KamataEngine::Vector3 kHudPos = {-3.5f, 3.8f, 0.0f};
		static inline const KamataEngine::Vector3 kHudScale = {1.2f, 0.2f, 0.1f};
		static inline const KamataEngine::Vector3 kComboPos = {1.0f, 3.5f, 0.0f};
		
		/*
		// --- プレイヤー設定 ---
		*/

		static inline const float kPlayerMeleeRange = 3.5f;     // 近接攻撃の届く距離
		static inline const int kChargeAttackHitCount = 5;      // チャージ攻撃のヒット数
		// static inline const float kChargeDamageInterval = 0.1f;

		/*
		// --- 敵設定 ---
		*/

		static inline const int kEnemyCount = 3;              // 初期配置数
		static inline const float kEnemyScaleNormal = 0.4f;   // 通常敵サイズ
		static inline const float kEnemyScaleSmall = 0.3f;    // 飛行敵サイズ
		static inline const float kFlyingHeightOffset = 5.0f; // 飛行敵の高さ補正

		/*
		// --- 戦闘・スコア ---
		*/

		static inline const int kDamageNormal = 1;           // 通常ダメージ
		static inline const int kDamageSpecial = 9999;       // 必殺技ダメージ 修正必
		static inline const float kComboPointHit = 8.0f;     // ヒット時加点
		static inline const float kComboPointCharge = 15.0f; // チャージ攻撃加点
		static inline const float kComboPointKill = 12.0f;   // 撃破ボーナス
		static inline const float kHitStopDuration = 0.1f;   // ヒットストップ時間

		/*
		// --- スロー弾スキル ---
		*/

		static inline const int kRainBallCount = 10;    // 降ってくる弾の数
		static inline const float kSlowDuration = 2.0f; // スロー効果時間

		/*
		// --- 特殊攻撃 ---
		*/

		static inline const float kSpecialChargeTime = 3.0f;    // 溜め時間
		static inline const float kSpecialDashTime = 0.7f;      // 斬り時間
		static inline const float kSpecialHitInterval = 0.06f;  // 斬る間隔
		static inline const int kSpecialFinaleSlashCount = 200; // 最後の演出の数
	};

	/*
	// --- Updateの仲間たち ---
	*/

	void GenerateBlooks();
	void UpdateMapBlocks();
	void UpdatePlayerAction();
	void UpdateProjectiles();
	void UpdateEnemies();
	void CheckCollisions();
	void CheckAllCollisions();
	void UpdateHud();
	void UpdateSceneFlow();
	void UpdateSpecialMove(float deltaTime);
	void PerformSpecialDash(float deltaTime);

	enum class SpecialState { None, Charge, Dash, Finish };

	/*
	// --- メンバ変数 ---
	*/

	ScenePhase phase_ = ScenePhase::FadeIn;
	bool finished_ = false;
	Fade* fade_ = nullptr;

	SpecialState specialState_ = SpecialState::None;
	float specialTimer_ = 0.0f;
	float specialHitInterval_ = 0.0f;
	bool specialFinalSlashesSpawned_ = false;

	KamataEngine::Camera* camera_ = nullptr;
	KamataEngine::Camera* uiCamera_ = nullptr;
	bool isDebugCameraActive_ = false;
	KamataEngine::DebugCamera* debugCamera_ = nullptr;
	CameraController* cameraController_ = nullptr;

	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* modelCube_ = nullptr;
	KamataEngine::Model* modelSkyDome_ = nullptr;
	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;
	MapChipField* mapChipField_ = nullptr;
	KamataEngine::Model* model_ = nullptr;

	Player* player_ = nullptr;
	KamataEngine::Model* modelPlayer_ = nullptr;

	std::list<Enemy*> enemies_;
	KamataEngine::Model* modelEnemy_ = nullptr;
	KamataEngine::Model* modelHpBar_ = nullptr;
	KamataEngine::Model* modelHp_ = nullptr;

	std::vector<std::unique_ptr<Bullet>> bullets_;
	std::vector<std::unique_ptr<Bullet>> slowBalls_;
	KamataEngine::Model* modelBullet_ = nullptr;
	KamataEngine::Model* modelSlowBall_ = nullptr;

	std::vector<std::unique_ptr<SlashEffect>> SlashEffects_;
	KamataEngine::Model* modelZangeki_ = nullptr;

	DeathParticle deathParticle_;
	KamataEngine::Model* modelDeathParticle_ = nullptr;
	float particleCooldown_ = 0.0f;

	KamataEngine::WorldTransform worldTransformHudHpBar_;
	KamataEngine::WorldTransform worldTransformHudHp_;
	ComboRank comboRank_;

	int score_ = 0;
	std::array<KamataEngine::Model*, 10> modelNumbers_{};

	float hitStopTimer_ = 0.0f;
};