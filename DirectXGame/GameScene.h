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
	// ゲームクリアかどうか
	bool IsClear() const { return isClear_; }

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
		static inline const KamataEngine::Vector3 kComboPos = {5.0f, 3.5f, 0.0f};

		/*
		// --- プレイヤー設定 ---
		*/

		// 近接攻撃の届く距離
		static inline const float kPlayerMeleeRange = 3.5f;
		// チャージ攻撃のヒット数
		static inline const int kChargeAttackHitCount = 5;
		// static inline const float kChargeDamageInterval = 0.1f;

		/*
		// --- 敵設定 ---
		*/

		// 初期配置数 (最初は0にしてスポナーに任せる)
		static inline const int kEnemyCount = 0;
		// 通常敵サイズ
		static inline const float kEnemyScaleNormal = 0.4f;
		// 飛行敵サイズ
		static inline const float kEnemyScaleSmall = 0.3f;
		// 飛行敵の高さ補正
		static inline const float kFlyingHeightOffset = 5.0f;

		/*
		// --- 戦闘とスコア ---
		*/

		// 通常ダメージ
		static inline const int kDamageNormal = 1;
		// 必殺技ダメージ
		static inline const int kDamageSpecial = 9999;
		// ヒット時加点
		static inline const float kComboPointHit = 8.0f;
		// チャージ攻撃加点
		static inline const float kComboPointCharge = 15.0f;
		// 撃破ボーナス
		static inline const float kComboPointKill = 12.0f;
		// ヒットストップ時間
		static inline const float kHitStopDuration = 0.1f;

		/*
		// --- スロー弾スキル ---
		*/

		// 降ってくる弾の数
		static inline const int kRainBallCount = 10;
		// スロー効果時間
		static inline const float kSlowDuration = 2.0f;

		/*
		// --- 特殊攻撃 ---
		*/

		// 溜め時間
		static inline const float kSpecialChargeTime = 3.0f;
		// 斬り時間
		static inline const float kSpecialDashTime = 0.7f;
		// 斬る間隔
		static inline const float kSpecialHitInterval = 0.06f;
		// 最後の演出の数
		static inline const int kSpecialFinaleSlashCount = 200;

		/*
		// --- サバイバル設定 (NEW) ---
		*/

		// 生き残る時間（秒）
		static inline const float kSurvivalTimeLimit = 30.0f;
		// 敵が湧く間隔（秒）
		static inline const float kEnemySpawnInterval = 3.0f;
	};

	/*
	// --- Updateの仲間たち ---
	*/

	// マップブロックの生成
	void GenerateBlocks();
	// マップブロックの更新
	void UpdateMapBlocks();
	// プレイヤーの入力攻撃処理
	void UpdatePlayerAction();
	// 弾の移動と寿命管理
	void UpdateProjectiles();
	// 敵の更新と回復行動
	void UpdateEnemies();
	// 敵のスポーン管理 (NEW)
	void UpdateEnemySpawn();
	// 弾などの当たり判定
	void CheckCollisions();
	// プレイヤーと敵の衝突判定
	void CheckAllCollisions();
	// UIの更新
	void UpdateHud();
	// フェードやシーン遷移の管理
	void UpdateSceneFlow();
	// 特殊攻撃の全体制御
	void UpdateSpecialMove(float deltaTime);
	// 特殊攻撃中のダッシュ処理
	void PerformSpecialDash(float deltaTime);

	enum class SpecialState {
		None,
		Charge, // プレイヤーだけ止まってチャージ
		Dash,   // 走る
		Finish  // 画面全体の斬撃演出
	};

	/*
	// --- メンバ変数 ---
	*/

	// シーン状態
	ScenePhase phase_ = ScenePhase::FadeIn;
	bool finished_ = false;
	bool isClear_ = false; // クリア判定 (NEW)
	Fade* fade_ = nullptr;

	// サバイバル用タイマー (NEW)
	float survivalTimer_ = 0.0f;
	float enemySpawnTimer_ = 0.0f;

	// 特殊攻撃用パラメータ
	SpecialState specialState_ = SpecialState::None;
	float specialTimer_ = 0.0f;
	float specialHitInterval_ = 0.0f;
	bool specialFinalSlashesSpawned_ = false;

	// カメラ
	KamataEngine::Camera* camera_ = nullptr;
	KamataEngine::Camera* uiCamera_ = nullptr;
	bool isDebugCameraActive_ = false;
	KamataEngine::DebugCamera* debugCamera_ = nullptr;
	CameraController* cameraController_ = nullptr;

	// マップと背景
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* modelCube_ = nullptr;
	KamataEngine::Model* modelSkyDome_ = nullptr;
	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;
	MapChipField* mapChipField_ = nullptr;
	KamataEngine::Model* model_ = nullptr;

	// プレイヤー
	Player* player_ = nullptr;
	KamataEngine::Model* modelPlayer_ = nullptr;

	// 敵
	std::list<Enemy*> enemies_;
	KamataEngine::Model* modelEnemy_ = nullptr;
	KamataEngine::Model* modelHpBar_ = nullptr;
	KamataEngine::Model* modelHp_ = nullptr;

	// 弾
	std::vector<std::unique_ptr<Bullet>> bullets_;
	std::vector<std::unique_ptr<Bullet>> slowBalls_;
	KamataEngine::Model* modelBullet_ = nullptr;
	KamataEngine::Model* modelSlowBall_ = nullptr;

	// エフェクト
	std::vector<std::unique_ptr<SlashEffect>> slashEffects_;
	KamataEngine::Model* modelZangeki_ = nullptr;

	DeathParticle deathParticle_;
	KamataEngine::Model* modelDeathParticle_ = nullptr;
	float particleCooldown_ = 0.0f;

	// UI
	KamataEngine::WorldTransform worldTransformHudHpBar_;
	KamataEngine::WorldTransform worldTransformHudHp_;
	ComboRank comboRank_;
	int comboIndex_ = 0;      // 今何連撃目か
	float comboTimer_ = 0.0f; // 次の攻撃までの受付時間

	// スコア
	int score_ = 0;
	std::array<KamataEngine::Model*, 10> modelNumbers_{};

	float hitStopTimer_ = 0.0f;
};