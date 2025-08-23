#include "GameClearScene.h"
#include "GameOverScene.h"
#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScene.h"

using namespace KamataEngine;

// ======== グローバル ========
DirectXCommon* dxCommon = nullptr;
TitleScene* titleScene = nullptr;
GameScene* gameScene = nullptr;
GameOverScene* gameOverScene = nullptr;
GameClearScene* gameClearScene = nullptr;

// シーン状態管理用enum
enum class Scene {
	kUnknown = 0,
	kTitle,
	kGame,
	kGameOver,
	kGameClear,
};

Scene scene = Scene::kUnknown;

// ======== シーン更新 ========
void UpdateScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Update();
		break;
	case Scene::kGame:
		gameScene->Update();
		break;
	case Scene::kGameOver:
		gameOverScene->Update();
		break;
	case Scene::kGameClear:
		gameClearScene->Update();
		break;
	default:
		break;
	}
}

// ======== シーン描画 ========
void DrawScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Draw();
		break;
	case Scene::kGame:
		gameScene->Draw();
		break;
	case Scene::kGameOver:
		gameOverScene->Draw();
		break;
	case Scene::kGameClear:
		gameClearScene->Draw();
		break;
	default:
		break;
	}
}

// ======== シーン切替 ========
void ChangeScene() {
	switch (scene) {
	case Scene::kTitle:
		if (titleScene->IsFinished()) {
			delete titleScene;
			titleScene = nullptr;
			scene = Scene::kGame;
			gameScene = new GameScene();
			gameScene->Initialize();
		}
		break;

	case Scene::kGame:
		if (gameScene->IsFinished()) {
			// ★ ここでEndStatusに応じて分岐
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
				// 念のため：不明ならタイトルへ
				scene = Scene::kTitle;
				titleScene = new TitleScene();
				titleScene->Initialize();
			}
		}
		break;

	case Scene::kGameOver:
		if (gameOverScene->IsFinished()) {
			delete gameOverScene;
			gameOverScene = nullptr;
			scene = Scene::kTitle;
			titleScene = new TitleScene();
			titleScene->Initialize();
		}
		break;

	case Scene::kGameClear:
		if (gameClearScene->IsFinished()) {
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
	KamataEngine::Initialize(L"LE2D_02_アオヤギ_ガクト_立体対決");
	dxCommon = DirectXCommon::GetInstance();

	// 最初はタイトル
	scene = Scene::kTitle;
	titleScene = new TitleScene();
	titleScene->Initialize();

	while (true) {
		if (KamataEngine::Update())
			break;

		dxCommon->PreDraw();

		UpdateScene();
		ChangeScene();
		DrawScene();

		dxCommon->PostDraw();
	}

	delete titleScene;
	delete gameScene;
	delete gameOverScene;
	delete gameClearScene;

	KamataEngine::Finalize();
	return 0;
}
