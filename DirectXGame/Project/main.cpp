#include "GameClearScene.h"
#include "GameOverScene.h"
#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScene.h"
#include "SoundManager.h"

using namespace KamataEngine;

// ==========================================================================
// グローバル
// ==========================================================================

DirectXCommon* dxCommon = nullptr;
TitleScene* titleScene = nullptr;
GameScene* gameScene = nullptr;
GameClearScene* gameClearScene = nullptr;
GameOverScene* gameOverScene = nullptr;
SoundManager* soundManager = nullptr;

// シーン状態管理用enum
enum class Scene {
	kUnknown = 0,
	kTitle,
	kGame,
	kClear,
	kOver,
};

Scene scene = Scene::kUnknown;

// ==========================================================================
// シーン更新
// ==========================================================================

void UpdateScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Update();
		break;
	case Scene::kGame:
		gameScene->Update();
		break;
	case Scene::kClear:
		gameClearScene->Update();
		break;
	case Scene::kOver:
		gameOverScene->Update();
		break;
	}
}

// ==========================================================================
// シーン描画
// ==========================================================================

void DrawScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Draw();
		break;
	case Scene::kGame:
		gameScene->Draw();
		break;
	case Scene::kClear:
		gameClearScene->Draw();
		break;
	case Scene::kOver:
		gameOverScene->Draw();
		break;
	}
}

// ==========================================================================
// シーン切替
// ==========================================================================

void ChangeScene() {
	switch (scene) {
	case Scene::kTitle:
		if (titleScene->IsFinished()) {
			delete titleScene;
			titleScene = nullptr;
			soundManager->ChangeBgm(SceneType::Game);
			scene = Scene::kGame;
			gameScene = new GameScene();
			gameScene->Initialize();
		}
		break;
	case Scene::kGame:
		if (gameScene->IsFinished()) {
			// クリア判定
			bool isClear = gameScene->IsClear();
			int finalScore = gameScene->GetScore();

			delete gameScene;
			gameScene = nullptr;

			if (isClear) {
				soundManager->ChangeBgm(SceneType::Clear);
				scene = Scene::kClear;
				gameClearScene = new GameClearScene();
				gameClearScene->Initialize(finalScore);
			} else {
				soundManager->ChangeBgm(SceneType::GameOver);
				scene = Scene::kOver;
				gameOverScene = new GameOverScene();
				gameOverScene->Initialize();
			}
		}
		break;
	case Scene::kClear:
		if (gameClearScene->IsFinished()) {
			delete gameClearScene;
			gameClearScene = nullptr;
			soundManager->ChangeBgm(SceneType::Title);
			scene = Scene::kTitle;
			titleScene = new TitleScene();
			titleScene->Initialize();
		}
		break;
	case Scene::kOver:
		if (gameOverScene->IsFinished()) {
			delete gameOverScene;
			gameOverScene = nullptr;
			soundManager->ChangeBgm(SceneType::Title);
			scene = Scene::kTitle;
			titleScene = new TitleScene();
			titleScene->Initialize();
		}
		break;
	}
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	KamataEngine::Initialize(L"LE2C_01_アオヤギ_ガクト_生き残り大作戦");
	dxCommon = DirectXCommon::GetInstance();

	// 最初はタイトル
	scene = Scene::kTitle;
	titleScene = new TitleScene();
	titleScene->Initialize();

	soundManager = new SoundManager();
	soundManager->Initialize();
	soundManager->ChangeBgm(SceneType::Title);

	while (true) {
		if (KamataEngine::Update()) {
			break;
		}

		dxCommon->PreDraw();

		UpdateScene();
		ChangeScene();
		DrawScene();

		dxCommon->PostDraw();
	}

	delete titleScene;
	delete gameScene;
	delete gameClearScene;
	delete gameOverScene;
	delete soundManager;

	KamataEngine::Finalize();
	return 0;
}
