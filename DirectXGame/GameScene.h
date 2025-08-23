#pragma once
#include "CameraController.h"
#include "DeathParticle.h"
#include "Enemy.h"
#include "Fade.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Player.h"
#include "Skydome.h"
#include "TypingChallenge.h"

#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

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
	void CheckGoalCollision();
	Vector3 AabbCenter(const AABB& aabb) { return {(aabb.min.x + aabb.max.x) * 0.5f, (aabb.min.y + aabb.max.y) * 0.5f, (aabb.min.z + aabb.max.z) * 0.5f}; }

	// ==== 単語OBJ（出題用） ====
	Model* LoadWordModel(const std::string& word); // 単語OBJを読み込み（キャッシュ）
	void UpdateWordTransformFollowPlayer();        // 単語OBJの追従位置更新

	// フェーズ
	enum class Phase { kFadeIn, kPlay, kTyping, kFadeOut, kClearFadeOut };
	Phase phase_ = Phase::kFadeIn;

	// モデル
	Model* modelCube_ = nullptr;
	Model* modelSkyDome_ = nullptr;
	Model* model_ = nullptr;
	Model* modelPlayer_ = nullptr;
	Model* modelEnemy_ = nullptr;
	Model* modelDeathParticle_ = nullptr;

	// スカイドーム用ワールド
	WorldTransform worldTransform_;

	// カメラ
	Camera* camera_ = nullptr;
	bool isDebugCameraActive_ = false;
	DebugCamera* debugCamera_ = nullptr;

	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;
	MapChipField* mapChipField_ = nullptr;

	Player* player_ = nullptr;
	std::list<Enemy*> enemies_;
	CameraController* cameraController_ = nullptr;

	// デスパーティクル
	DeathParticle deathParticle_;
	float particleCooldown_ = 0.0f;

	// シーン終了フラグ
	bool finished_ = false;

	// フェード
	Fade* fade_ = nullptr;

	// ===== タイピング勝負 =====
	TypingChallenge typing_;
	Enemy* typingTarget_ = nullptr;
	float typingTimeLimit_ = 10.0f;
	std::vector<std::string> typingWords_ = {"programming", "kamata", "vector", "matrix", "enemy", "player", "jump", "boost", "lockon", "beam", "apple", "sword"};

	// 現在の出題単語（OBJ）表示用
	std::unordered_map<std::string, Model*> wordCache_;
	WorldTransform wordTransform_;
	std::string currentTypingWord_;
	Vector3 typingAnchorOffset_ = {0.0f, 3.0f, 0.0f};
	std::string wordPrefix_ = "";
	std::string wordSuffix_ = "";
	// 単語ごとのリソース名オーバーライド（例：player → playerMoji）
	std::unordered_map<std::string, std::string> wordResOverride_;

	// ==== 単語入力ハイライト（黄色バー） ====
	Model* modelHighlight_ = nullptr; // 無ければ block を使用
	WorldTransform hlTransform_;      // 伸びるバー（入力済み）
	WorldTransform hlBackTransform_;  // 背景バー（全長）
	float hlFullWidth_ = 6.0f;        // 単語の想定全幅
	float hlHeight_ = 0.25f;          // バーの高さ
	float hlOffsetY_ = -1.2f;         // 単語の少し下に
	float hlOffsetZ_ = 0.0f;          // Zずらし無し（厚みで勝つ）

	// ===== ゴール =====
	WorldTransform goalTransform_;
	AABB goalAabb_{};
	bool mapCleared_ = false;
};
