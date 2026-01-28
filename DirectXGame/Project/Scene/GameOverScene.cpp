#include "GameOverScene.h"
#include "MyMath.h"
#include <cmath>
#include <numbers>

using namespace KamataEngine;

void GameOverScene::Initialize() {
	textModel_ = Model::CreateFromOBJ("gameover");
	textTransform_.Initialize();
	textTransform_.translation_ = {0.0f, 2.0f, 0.0f};
	textTransform_.scale_ = {10.0f, 10.0f, 10.0f};
	textColor_.Initialize();
	textColor_.SetColor({0.0f, 0.0f, 0.0f, 1.0f});

	skyDomeModel_ = Model::CreateFromOBJ("SkyDome", true);
	skyDomeWT_.Initialize();
	skyDomeWT_.scale_ = {50.0f, 50.0f, 50.0f};

	pressSpaceModel_ = Model::CreateFromOBJ("PressSpace");
	pressSpaceTransform_.Initialize();
	pressSpaceTransform_.translation_ = {0.0f, -4.0f, 0.0f};
	pressSpaceTransform_.scale_ = {5.0f, 5.0f, 5.0f};
	pressSpaceColor_.Initialize();
	pressSpaceColor_.SetColor({0.0f, 0.0f, 0.0f, 1.0f});

	camera_ = new Camera();
	camera_->Initialize();

	fade_ = new Fade();
	fade_->Initialize();
	phase_ = Phase::FadeIn;
	fade_->Start(Fade::Status::FadeIn, 0.8f);

	finished_ = false;
}

void GameOverScene::Update() {
	switch (phase_) {
	case Phase::FadeIn:
		fade_->Update();
		if (fade_->IsFinished()) {
			phase_ = Phase::Main;
		}

		break;

	case Phase::Main:
		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			phase_ = Phase::FadeOut;
			fade_->Start(Fade::Status::FadeOut, 0.8f);
		}

		break;

	case Phase::FadeOut:
		fade_->Update();
		if (fade_->IsFinished()) {
			finished_ = true;
		}

		break;
	}

	blinkTimer_ += 1.0f / 60.0f;
	
	if (blinkTimer_ > 0.5f) {
		blinkVisible_ = !blinkVisible_;
		blinkTimer_ = 0.0f;
	}

	textTransform_.matWorld_ = MakeAffineMatrix(textTransform_.scale_, textTransform_.rotation_, textTransform_.translation_);
	textTransform_.TransferMatrix();
	pressSpaceTransform_.matWorld_ = MakeAffineMatrix(pressSpaceTransform_.scale_, pressSpaceTransform_.rotation_, pressSpaceTransform_.translation_);
	pressSpaceTransform_.TransferMatrix();
}

void GameOverScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	Model::PreDraw(dxCommon->GetCommandList());
	
	if (skyDomeModel_) {
		skyDomeModel_->Draw(skyDomeWT_, *camera_);
	}

	if (textModel_) {
		textModel_->Draw(textTransform_, *camera_, &textColor_);
	}

	if (pressSpaceModel_ && blinkVisible_) {
		pressSpaceModel_->Draw(pressSpaceTransform_, *camera_, &pressSpaceColor_);
	}

	Model::PostDraw();
	
	if (fade_) {
		fade_->Draw();
	}
}

GameOverScene::~GameOverScene() {
	delete textModel_;
	delete skyDomeModel_;
	delete pressSpaceModel_;
	delete camera_;
	delete fade_;
}