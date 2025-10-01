#include "GameClearScene.h"
#include "GameOverScene.h"
#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScene.h"
#include "TutorialScene.h"

using namespace KamataEngine;

DirectXCommon* dxCommon = nullptr;

TitleScene* titleScene = nullptr;
GameScene* gameScene = nullptr;
TutorialScene* tutorialScene = nullptr;
GameOverScene* gameOverScene = nullptr;
GameClearScene* gameClearScene = nullptr;

enum class Scene {
	kUnknown = 0,
	kTitle,
	kGame,
	kTutorial,
	kGameOver,
	kGameClear,
};

Scene scene = Scene::kUnknown;

// ======== シーン更新 ========
static inline void UpdateScene() {
	switch (scene) {
	case Scene::kTitle:
		if (titleScene)
			titleScene->Update();
		break;
	case Scene::kGame:
		if (gameScene)
			gameScene->Update();
		break;
	case Scene::kTutorial:
		if (tutorialScene)
			tutorialScene->Update();
		break;
	case Scene::kGameOver:
		if (gameOverScene)
			gameOverScene->Update();
		break;
	case Scene::kGameClear:
		if (gameClearScene)
			gameClearScene->Update();
		break;
	default:
		break;
	}
}

// ======== シーン描画 ========
static inline void DrawScene() {
	switch (scene) {
	case Scene::kTitle:
		if (titleScene)
			titleScene->Draw();
		break;
	case Scene::kGame:
		if (gameScene)
			gameScene->Draw();
		break;
	case Scene::kTutorial:
		if (tutorialScene)
			tutorialScene->Draw();
		break;
	case Scene::kGameOver:
		if (gameOverScene)
			gameOverScene->Draw();
		break;
	case Scene::kGameClear:
		if (gameClearScene)
			gameClearScene->Draw();
		break;
	default:
		break;
	}
}

// ======== シーン切替 ========
static inline void ChangeScene() {
	switch (scene) {
	// ---- Title → Game or Tutorial ----
	case Scene::kTitle:
		if (titleScene && titleScene->IsFinished()) {
			TitleScene::NextAction act = titleScene->GetNextAction();
			delete titleScene;
			titleScene = nullptr;

			if (act == TitleScene::NextAction::OpenTutorial) {
				scene = Scene::kTutorial;
				tutorialScene = new TutorialScene();
				tutorialScene->Initialize();
			} else { // StartGame
				scene = Scene::kGame;
				gameScene = new GameScene();
				gameScene->Initialize();
			}
		}
		break;

	// ---- Game → GameOver or GameClear ----
	case Scene::kGame:
		if (gameScene && gameScene->IsFinished()) {
			GameScene::EndStatus st = gameScene->GetEndStatus();
			delete gameScene;
			gameScene = nullptr;

			if (st == GameScene::EndStatus::GameOver) {
				scene = Scene::kGameOver;
				gameOverScene = new GameOverScene();
				gameOverScene->Initialize();
			} else if (st == GameScene::EndStatus::GameClear) {
				scene = Scene::kGameClear;
				gameClearScene = new GameClearScene();
				gameClearScene->Initialize();
			} else {
				// フォールバック：タイトルへ
				scene = Scene::kTitle;
				titleScene = new TitleScene();
				titleScene->Initialize();
			}
		}
		break;

	// ---- Tutorial → Title ----
	case Scene::kTutorial:
		if (tutorialScene && tutorialScene->IsFinished()) {
			delete tutorialScene;
			tutorialScene = nullptr;
			scene = Scene::kTitle;
			titleScene = new TitleScene();
			titleScene->Initialize();
		}
		break;

	// ---- GameOver → Title ----
	case Scene::kGameOver:
		if (gameOverScene && gameOverScene->IsFinished()) {
			delete gameOverScene;
			gameOverScene = nullptr;
			scene = Scene::kTitle;
			titleScene = new TitleScene();
			titleScene->Initialize();
		}
		break;

	// ---- GameClear → Title ----
	case Scene::kGameClear:
		if (gameClearScene && gameClearScene->IsFinished()) {
			delete gameClearScene;
			gameClearScene = nullptr;
			scene = Scene::kTitle;
			titleScene = new TitleScene();
			titleScene->Initialize();
		}
		break;

	default:
		break;
	}
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
  
	KamataEngine::Initialize(L"LE2C_01_アオヤギ_ガクト_AL4");
	
  dxCommon = DirectXCommon::GetInstance();

	// 最初はタイトル
	scene = Scene::kTitle;
	titleScene = new TitleScene();
	titleScene->Initialize();

	while (true) {
		if (KamataEngine::Update())
			break;

		dxCommon->PreDraw();

		// 1) 更新
		UpdateScene();
		// 2) 状態に応じて切替
		ChangeScene();
		// 3) 描画
		DrawScene();

		dxCommon->PostDraw();
	}

	// お片付け（存在チェックしてから削除）
	delete titleScene;
	titleScene = nullptr;
	delete gameScene;
	gameScene = nullptr;
	delete tutorialScene;
	tutorialScene = nullptr;
	delete gameOverScene;
	gameOverScene = nullptr;
	delete gameClearScene;
	gameClearScene = nullptr;

	KamataEngine::Finalize();
	return 0;
}
