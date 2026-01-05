#include "TitleScene.h"
#include "MyMath.h"
#include <cmath>

using namespace KamataEngine;

TitleScene::TitleScene() {}

TitleScene::~TitleScene() {
	delete titleFontModel_;
	delete playerModel_;
	delete skyDomeModel_;
	delete pressSpaceModel_;
	delete camera_;
	delete fade_;
	delete tKeyModel_;
	delete tutorialModel_;
}

void TitleScene::Initialize() {
	// タイトルロゴ
	titleFontModel_ = Model::CreateFromOBJ("titleFont");
	titleTransform_.Initialize();
	titleTransform_.translation_ = {0.0f, 1.0f, 0.0f};
	titleTransform_.scale_ = {2.0f, 2.0f, 2.0f};
	titleColor_.Initialize();
	titleColor_.SetColor({0.0f, 0.0f, 0.0f, 1.0f});

	// 画面下にプレイヤーモデルを置いて装飾
	playerModel_ = Model::CreateFromOBJ("player");
	playerTransform_.Initialize();
	playerTransform_.translation_ = {0.0f, -2.0f, 0.0f};
	playerTransform_.scale_ = {10.0f, 10.0f, 10.0f};
	playerTransform_.rotation_ = {0.0f, 3.14f, 0.0f};

	// 天球
	skyDomeModel_ = Model::CreateFromOBJ("SkyDome", true);
	skyDomeWT_.Initialize();
	skyDomeWT_.scale_ = {50.0f, 50.0f, 50.0f};

	// 文字
	pressSpaceModel_ = Model::CreateFromOBJ("PressSpace");
	pressSpaceWT_.Initialize();
	pressSpaceWT_.translation_ = {0.0f, -14.0f, 0.0f};
	pressSpaceWT_.scale_ = {5.0f, 5.0f, 5.0f};
	pressSpaceColor_.Initialize();
	pressSpaceColor_.SetColor({0.0f, 0.0f, 0.0f, 1.0f});
	
	tKeyModel_ = Model::CreateFromOBJ("Tkey");
	tKeyWT_.Initialize();
	tKeyWT_.translation_ = {0.0f, -18.0f, 0.0f};
	tKeyWT_.scale_ = {2.0f, 2.0f, 2.0f};
	tKeyColor_.Initialize();
	tKeyColor_.SetColor({0.0f, 0.0f, 0.0f, 1.0f});

	tutorialModel_ = Model::CreateFromOBJ("setumei");
	tutorialWT_.Initialize();
	tutorialWT_.translation_ = {0.0f, 0.0f, 0.0f};
	tutorialWT_.scale_ = {2.0f, 2.0f, 2.0f};

	isTutorialMode_ = false;

	// カメラ
	camera_ = new Camera();
	camera_->Initialize();

	// フェード
	fade_ = new Fade();
	fade_->Initialize();
	phase_ = ScenePhase::FadeIn;
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	// 状態初期化
	blinkTimer_ = 0.0f;
	blinkVisible_ = true;
	logoMoveTimer_ = 0.0f;
	finished_ = false;
}

void TitleScene::Update() {
	switch (phase_) {
	case ScenePhase::FadeIn:
		fade_->Update();
		if (fade_->IsFinished()) {
			phase_ = ScenePhase::Play;
		}

		break;

	case ScenePhase::Play:
		if (isTutorialMode_) {
			// チュートリアル表示中

			// TキーまたはSPACEキーで閉じる
			if (Input::GetInstance()->TriggerKey(DIK_T) || Input::GetInstance()->TriggerKey(DIK_SPACE)) {
				isTutorialMode_ = false;
			}
		} else {
			// 通常時

			// Tキーでチュートリアルを開く
			if (Input::GetInstance()->TriggerKey(DIK_T)) {
				isTutorialMode_ = true;
			}
			// SPACEキーでゲーム開始
			else if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
				phase_ = ScenePhase::FadeOut;
				fade_->Start(Fade::Status::FadeOut, 1.0f);
			}
		}
		break;

	case ScenePhase::FadeOut:
		fade_->Update();
		if (fade_->IsFinished()) {
			finished_ = true;
		}
		break;
	}

	// 点滅
	blinkTimer_ += 1.0f / 60.0f;
	if (blinkTimer_ > 0.5f) {
		blinkVisible_ = !blinkVisible_;
		blinkTimer_ = 0.0f;
	}

	// ロゴ上下ゆれ
	logoMoveTimer_ += 1.0f / 60.0f;
	float baseY = 10.0f;
	float offsetY = std::sin(logoMoveTimer_ * 2.0f) * 0.2f;
	titleTransform_.translation_.y = baseY + offsetY;

	// 行列更新
	titleTransform_.matWorld_ = MakeAffineMatrix(titleTransform_.scale_, titleTransform_.rotation_, titleTransform_.translation_);
	titleTransform_.TransferMatrix();
	playerTransform_.matWorld_ = MakeAffineMatrix(playerTransform_.scale_, playerTransform_.rotation_, playerTransform_.translation_);
	playerTransform_.TransferMatrix();
	pressSpaceWT_.matWorld_ = MakeAffineMatrix(pressSpaceWT_.scale_, pressSpaceWT_.rotation_, pressSpaceWT_.translation_);
	pressSpaceWT_.TransferMatrix();
	tKeyWT_.matWorld_ = MakeAffineMatrix(tKeyWT_.scale_, tKeyWT_.rotation_, tKeyWT_.translation_);
	tKeyWT_.TransferMatrix();
	tutorialWT_.matWorld_ = MakeAffineMatrix(tutorialWT_.scale_, tutorialWT_.rotation_, tutorialWT_.translation_);
	tutorialWT_.TransferMatrix();
}

void TitleScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	Model::PreDraw(dxCommon->GetCommandList());

	if (skyDomeModel_) {
		skyDomeModel_->Draw(skyDomeWT_, *camera_);
	}
	if (!isTutorialMode_) {
		titleFontModel_->Draw(titleTransform_, *camera_, &titleColor_);
		playerModel_->Draw(playerTransform_, *camera_);
	}

	if (!isTutorialMode_ && blinkVisible_) {
		if (pressSpaceModel_) {
			pressSpaceModel_->Draw(pressSpaceWT_, *camera_, &pressSpaceColor_);
		}

		if (tKeyModel_) {
			tKeyModel_->Draw(tKeyWT_, *camera_, &tKeyColor_);
		}
	}

	if (isTutorialMode_ && tutorialModel_) {
		tutorialModel_->Draw(tutorialWT_, *camera_);
	}

	Model::PostDraw();

	if (fade_) {
		fade_->Draw();
	}
}
