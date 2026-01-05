#include "GameClearScene.h"
#include "MyMath.h"
#include <cmath>
#include <numbers>

using namespace KamataEngine;

void GameClearScene::Initialize(int score) {
	// モデル生成
	textModel_ = Model::CreateFromOBJ("gameclear");
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

	for (int i = 0; i < 10; ++i) {
		modelNumbers_[i] = Model::CreateFromOBJ(std::to_string(i));
	}

	score_ = score;

	// スコア表示位置の初期化
	for (auto& wt : scoreWTs_) {
		wt.Initialize();
		wt.scale_ = {5.0f, 5.0f, 5.0f};

		wt.rotation_.y = std::numbers::pi_v<float> / -2.0f;
	}

	camera_ = new Camera();
	camera_->Initialize();

	fade_ = new Fade();
	fade_->Initialize();
	phase_ = Phase::FadeIn;
	fade_->Start(Fade::Status::FadeIn, 0.8f);

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

	// 点滅処理
	blinkTimer_ += 1.0f / 60.0f;
	if (blinkTimer_ > 0.5f) {
		blinkVisible_ = !blinkVisible_;
		blinkTimer_ = 0.0f;
	}

	std::string scoreStr = std::to_string(score_);
	float charWidth = 3.5f;
	float totalWidth = (scoreStr.length() - 1) * charWidth;
	float startX = -totalWidth * 0.5f;
	float posY = -15.0f;

	for (int i = 0; i < (int)scoreStr.length(); ++i) {
		if (i < scoreWTs_.size()) {
			scoreWTs_[i].translation_ = {startX + i * charWidth, posY, 0.0f};
			scoreWTs_[i].matWorld_ = MakeAffineMatrix(scoreWTs_[i].scale_, scoreWTs_[i].rotation_, scoreWTs_[i].translation_);
			scoreWTs_[i].TransferMatrix();
		}
	}

	// 行列更新
	textTransform_.matWorld_ = MakeAffineMatrix(textTransform_.scale_, textTransform_.rotation_, textTransform_.translation_);
	textTransform_.TransferMatrix();
	pressSpaceTransform_.matWorld_ = MakeAffineMatrix(pressSpaceTransform_.scale_, pressSpaceTransform_.rotation_, pressSpaceTransform_.translation_);
	pressSpaceTransform_.TransferMatrix();

}

void GameClearScene::Draw() {
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

	std::string scoreStr = std::to_string(score_);
	for (int i = 0; i < (int)scoreStr.length(); ++i) {
		int digit = scoreStr[i] - '0';
		if (digit >= 0 && digit <= 9 && i < scoreWTs_.size()) {
			modelNumbers_[digit]->Draw(scoreWTs_[i], *camera_);
		}
	}

	Model::PostDraw();
	if (fade_) {
		fade_->Draw();
	}
}

GameClearScene::~GameClearScene() {
	delete textModel_;
	delete skyDomeModel_;
	delete pressSpaceModel_;
	delete camera_;
	delete fade_;
	for (auto* m : modelNumbers_) {
		delete m;
	}
}