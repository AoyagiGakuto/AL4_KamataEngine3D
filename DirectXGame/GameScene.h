#pragma once
#include "CameraController.h"
#include "ZangekiEffect.h"
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

using namespace KamataEngine;

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

	// フェーズを追加：入る/遊ぶ/出る
	ScenePhase phase_ = ScenePhase::FadeIn;

	// モデル
	Model* modelCube_ = nullptr;
	Model* modelSkyDome_ = nullptr;
	Model* model_ = nullptr;
	Model* modelPlayer_ = nullptr;
	Model* modelEnemy_ = nullptr;
	Model* modelDeathParticle_ = nullptr;
	Model* modelBullet_ = nullptr;
	Model* modelSlowBall_ = nullptr;
	Model* modelZangeki_ = nullptr;
	Model* modelHpBar_ = nullptr;
	Model* modelHp_ = nullptr;

	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	WorldTransform worldTransformHudHpBar_;
	WorldTransform worldTransformHudHp_;

	// カメラ
	Camera* camera_;
	Camera* uiCamera_ = nullptr;
	bool isDebugCameraActive_ = false;
	DebugCamera* debugCamera_ = nullptr;

	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;
	MapChipField* mapChipField_ = nullptr;

	Player* player_ = nullptr;
	std::list<Enemy*> enemies_;
	CameraController* cameraController_ = nullptr;

	std::vector<std::unique_ptr<Bullet>> bullets_;
	std::vector<std::unique_ptr<Bullet>> slowBalls_;

	std::array<Model*, 10> modelNumbers_{};
	std::vector<std::unique_ptr<HitEffect>> hitEffects_;
	std::vector<std::unique_ptr<ZangekiEffect>> zangekiEffects_;
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
