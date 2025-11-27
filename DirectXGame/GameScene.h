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
#include "HitEffect.h"
#include <memory>
#include <vector>
#include <array>

class GameScene {
public:
	~GameScene();

	void Initialize();
	void Update();
	void Draw();
	bool IsFinished() const { return finished_; }

private:
	void GenerateBlooks();
	void CheckAllCollisions();
	void UpdateSpecialMove(float deltaTime);
	void PerformSpecialDash(float deltaTime);

	enum class SpecialState {
		None,
		Charge, // プレイヤーだけ止まってチャージ
		Dash,   // 次元斬
		Finish  // 画面全体の斬撃演出
	};

	SpecialState specialState_ = SpecialState::None;
	float specialTimer_ = 0.0f;       // 各フェーズの残り時間
	float specialHitInterval_ = 0.0f; // 何フレームごとに敵を斬るか
	bool specialFinalSlashesSpawned_ = false;

	ScenePhase phase_ = ScenePhase::FadeIn;

	// モデル
	KamataEngine::Model* modelCube_ = nullptr;
	KamataEngine::Model* modelSkyDome_ = nullptr;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* modelPlayer_ = nullptr;
	KamataEngine::Model* modelEnemy_ = nullptr;
	KamataEngine::Model* modelDeathParticle_ = nullptr;
	KamataEngine::Model* modelBullet_ = nullptr;
	KamataEngine::Model* modelSlowBall_ = nullptr;
	KamataEngine::Model* modelZangeki_ = nullptr;
	KamataEngine::Model* modelHpBar_ = nullptr;
	KamataEngine::Model* modelHp_ = nullptr;

	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::WorldTransform worldTransformHudHpBar_;
	KamataEngine::WorldTransform worldTransformHudHp_;

	// カメラ
	KamataEngine::Camera* camera_;
	KamataEngine::Camera* uiCamera_ = nullptr;
	bool isDebugCameraActive_ = false;
	KamataEngine::DebugCamera* debugCamera_ = nullptr;

	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;
	MapChipField* mapChipField_ = nullptr;

	Player* player_ = nullptr;
	std::list<Enemy*> enemies_;
	CameraController* cameraController_ = nullptr;

	std::vector<std::unique_ptr<Bullet>> bullets_;
	std::vector<std::unique_ptr<Bullet>> slowBalls_;

	std::array<KamataEngine::Model*, 10> modelNumbers_{};
	std::vector<std::unique_ptr<HitEffect>> hitEffects_;
	std::vector<std::unique_ptr<SlashEffect>> SlashEffects_;
	int score_ = 0;

	// デスパーティクル
	DeathParticle deathParticle_;
	float particleCooldown_ = 0.0f;

	// シーン終了フラグ
	bool finished_ = false;

	// シーン内フェード
	Fade* fade_ = nullptr;

	float hitStopTimer_ = 0.0f;
	
	// コンボランク
	ComboRank comboRank_;
};
