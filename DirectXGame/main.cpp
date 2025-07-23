#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScene.h"

using namespace KamataEngine;

// ======== グローバル ========
DirectXCommon* dxCommon = nullptr;
TitleScene* titleScene = nullptr;
GameScene* gameScene = nullptr;

// シーン状態管理用enum
enum class Scene {
	kUnknown = 0,
	kTitle,
	kGame,
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
			delete gameScene;
			gameScene = nullptr;

			scene = Scene::kTitle;
			titleScene = new TitleScene();
			titleScene->Initialize();
		}
		break;
	}
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	KamataEngine::Initialize(L"LE2D_02_アオヤギ_ガクト_AL3");
	dxCommon = DirectXCommon::GetInstance();

	// ★最初はタイトルシーンから
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

	// シーン解放
	delete titleScene;
	delete gameScene;

	KamataEngine::Finalize();
	return 0;
}
