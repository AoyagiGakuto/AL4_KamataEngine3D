#pragma once
#include "CameraController.h"
#include "SlashEffect.h"
#include "DeathParticle.h"
#include "ScenePhase.h"
#include "Enemy.h"
#include "Fade.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Bullet.h"
#include "Player.h"
#include "Skydome.h"
#include "ComboRank.h"

#include <memory>
#include <vector>
#include <array>

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
	// --- Updateの仲間たち ---
	*/

	// マップブロックの生成
	void GenerateBlooks();
	// マップブロックの更新
	void UpdateMapBlocks();
	// プレイヤーの入力攻撃処理
	void UpdatePlayerAction();
	// 弾の移動と寿命管理
	void UpdateProjectiles();
	// 敵の更新と回復行動
	void UpdateEnemies();
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
	Fade* fade_ = nullptr;

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

	// マップ・背景
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* modelCube_ = nullptr;
	KamataEngine::Model* modelSkyDome_ = nullptr;
	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;
	MapChipField* mapChipField_ = nullptr;
	KamataEngine::Model* model_ = nullptr; // 汎用

	// プレイヤー
	Player* player_ = nullptr;
	KamataEngine::Model* modelPlayer_ = nullptr;

	// 敵
	std::list<Enemy*> enemies_;
	KamataEngine::Model* modelEnemy_ = nullptr;
	KamataEngine::Model* modelHpBar_ = nullptr;
	KamataEngine::Model* modelHp_ = nullptr; // 敵HP用

	// 弾
	std::vector<std::unique_ptr<Bullet>> bullets_;
	std::vector<std::unique_ptr<Bullet>> slowBalls_;
	KamataEngine::Model* modelBullet_ = nullptr;
	KamataEngine::Model* modelSlowBall_ = nullptr;

	// エフェクト
	std::vector<std::unique_ptr<SlashEffect>> SlashEffects_;
	KamataEngine::Model* modelZangeki_ = nullptr;

	DeathParticle deathParticle_;
	KamataEngine::Model* modelDeathParticle_ = nullptr;
	float particleCooldown_ = 0.0f;

	// UI
	KamataEngine::WorldTransform worldTransformHudHpBar_;
	KamataEngine::WorldTransform worldTransformHudHp_;
	ComboRank comboRank_;

	// スコア数字モデル
	int score_ = 0;
	std::array<KamataEngine::Model*, 10> modelNumbers_{};

	float hitStopTimer_ = 0.0f;
};
