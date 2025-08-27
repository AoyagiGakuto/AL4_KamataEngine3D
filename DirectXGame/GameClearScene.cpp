#include "GameClearScene.h"
#include "MyMath.h"
#include <cmath>
#include <numbers>

void GameClearScene::Initialize() {
	// 「GAME CLEAR」OBJ
	textModel_ = Model::CreateFromOBJ("gameclear");
	textTransform_.Initialize();
	textTransform_.translation_ = {0.0f, 5.0f, 6.0f};
	textTransform_.scale_ = {5.5f, 5.5f, 2.5f};

	// 背景OBJ
	backgroundModel_ = Model::CreateFromOBJ("background");
	backgroundTransform_.Initialize();
	backgroundTransform_.translation_ = {0.0f, 0.0f, 10.0f};
	backgroundTransform_.rotation_.y = std::numbers::pi_v<float>;
	backgroundTransform_.scale_ = {10000.0f, 10000.0f, 10.0f};

	skyDomeModel_ = Model::CreateFromOBJ("tenkixyuu", true);
	// skyDomeModel_ = Model::CreateFromOBJ("tenkixyuu"); skyDomeWT_.scale_.x *= -1.0f;
	skyDomeWT_.Initialize();
	skyDomeWT_.scale_ = {50.0f, 50.0f, 50.0f};
	skyDomeWT_.rotation_.y = std::numbers::pi_v<float>;
	skyDomeWT_.translation_ = {0.0f, 0.0f, 0.0f};
	skyDomeWT_.TransferMatrix();

	// PressSpace（点滅）
	pressSpaceModel_ = Model::CreateFromOBJ("PressSpace");
	pressSpaceTransform_.Initialize();
	pressSpaceTransform_.translation_ = {0.0f, -3.0f, 6.0f};
	pressSpaceTransform_.scale_ = {3.5f, 3.5f, 1.5f};

	// カメラ
	camera_ = new Camera();
	camera_->Initialize();
	camera_->translation_ = {0.0f, 0.0f, -15.0f};
	camera_->TransferMatrix();

	// フェード
	fade_ = new Fade();
	fade_->Initialize();
	phase_ = Phase::FadeIn;
	fade_->Start(Fade::Status::FadeIn, 0.8f);

	blinkTimer_ = 0.0f;
	blinkVisible_ = true;
	finished_ = false;
}

void GameClearScene::Update() {
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

	// 点滅
	blinkTimer_ += 1.0f / 60.0f;
	if (blinkTimer_ > 0.5f) {
		blinkVisible_ = !blinkVisible_;
		blinkTimer_ = 0.0f;
	}

	// 天球をカメラに追従
	skyDomeWT_.translation_ = camera_->translation_;
	skyDomeWT_.matWorld_ = MakeAffineMatrix(skyDomeWT_.scale_, skyDomeWT_.rotation_, skyDomeWT_.translation_);
	skyDomeWT_.TransferMatrix();

	// 行列更新
	if (textModel_) {
		textTransform_.matWorld_ = MakeAffineMatrix(textTransform_.scale_, textTransform_.rotation_, textTransform_.translation_);
		textTransform_.TransferMatrix();
	}
	backgroundTransform_.matWorld_ = MakeAffineMatrix(backgroundTransform_.scale_, backgroundTransform_.rotation_, backgroundTransform_.translation_);
	backgroundTransform_.TransferMatrix();

	pressSpaceTransform_.matWorld_ = MakeAffineMatrix(pressSpaceTransform_.scale_, pressSpaceTransform_.rotation_, pressSpaceTransform_.translation_);
	pressSpaceTransform_.TransferMatrix();
}

void GameClearScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	Model::PreDraw(dxCommon->GetCommandList());

	if (skyDomeModel_)
		skyDomeModel_->Draw(skyDomeWT_, *camera_);
	if (backgroundModel_)
		backgroundModel_->Draw(backgroundTransform_, *camera_);
	if (textModel_)
		textModel_->Draw(textTransform_, *camera_);
	if (pressSpaceModel_ && blinkVisible_)
		pressSpaceModel_->Draw(pressSpaceTransform_, *camera_);

	Model::PostDraw();
	if (fade_)
		fade_->Draw();
}

GameClearScene::~GameClearScene() {
	delete textModel_;
	delete backgroundModel_;
	delete skyDomeModel_;
	delete pressSpaceModel_;
	delete camera_;
	delete fade_;
}
