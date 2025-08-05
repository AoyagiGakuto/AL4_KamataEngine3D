#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScene.h"
#include "Fade.h"

using namespace KamataEngine;

// ======== グローバル ========
DirectXCommon* dxCommon = nullptr;
TitleScene* titleScene = nullptr;
GameScene* gameScene = nullptr;
Fade fade;
bool isSceneChanging = false;

// シーン状態管理用enum
enum class Scene {
	kUnknown = 0,
	kTitle,
	kGame,
};

Scene scene = Scene::kUnknown;

// ======== シーン更新 ========
void UpdateScene() {
	fade.Update(); // フェードの更新を最初に

	// フェード中はシーンのUpdateを止める
	if (fade.IsFinished() == false && fade.GetStatus() != Fade::Status::None) {
		return;
	}

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
	fade.Draw(); // 最後にフェードを描画
}

// ======== シーン切替 ========
void ChangeScene() {
	// シーン切り替え中は何もしない
	if (isSceneChanging) {
		// フェードアウトが終わったらシーン切り替え
		if (fade.IsFinished() && fade.GetStatus() == Fade::Status::FadeOut) {
			if (scene == Scene::kTitle) {
				delete titleScene;
				titleScene = nullptr;
				scene = Scene::kGame;
				gameScene = new GameScene();
				gameScene->Initialize();
			} else if (scene == Scene::kGame) {
				delete gameScene;
				gameScene = nullptr;
				scene = Scene::kTitle;
				titleScene = new TitleScene();
				titleScene->Initialize();
			}
			// フェードイン開始
			fade.Start(Fade::Status::FadeIn, 30.0f);
			isSceneChanging = false;
		}
		return;
	}

	switch (scene) {
	case Scene::kTitle:
		if (titleScene->IsFinished()) {
			fade.Start(Fade::Status::FadeOut, 30.0f); // フェードアウト開始
			isSceneChanging = true;
		}
		break;
	case Scene::kGame:
		if (gameScene->IsFinished()) {
			fade.Start(Fade::Status::FadeOut, 30.0f); // フェードアウト開始
			isSceneChanging = true;
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
	fade.Initialize();

	while (true) {
		if (KamataEngine::Update())
			break;

		dxCommon->PreDraw();

		ChangeScene();
		UpdateScene();
		DrawScene();

		dxCommon->PostDraw();
	}

	// シーン解放
	delete titleScene;
	delete gameScene;

	KamataEngine::Finalize();
	return 0;
}
