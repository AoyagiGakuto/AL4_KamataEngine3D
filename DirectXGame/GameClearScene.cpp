#include "GameClearScene.h"
#include "MyMath.h"

void GameClearScene::Initialize() {
	textModel_ = Model::CreateFromOBJ("GameClear");

	textTransform_.Initialize();
	textTransform_.translation_ = {0.0f, 3.0f, 0.0f};
	textTransform_.scale_ = {8.5f, 8.5f, 2.5f};

	camera_ = new Camera();
	camera_->Initialize();

	fade_ = new Fade();
	fade_->Initialize();
	phase_ = Phase::FadeIn;
	fade_->Start(Fade::Status::FadeIn, 0.8f);
}

void GameClearScene::Update() {
	switch (phase_) {
	case Phase::FadeIn:
		fade_->Update();
		if (fade_->IsFinished())
			phase_ = Phase::Main;
		break;
	case Phase::Main:
		// Space でタイトルへ
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

	textTransform_.matWorld_ = MakeAffineMatrix(textTransform_.scale_, textTransform_.rotation_, textTransform_.translation_);
	textTransform_.TransferMatrix();
}

void GameClearScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	Model::PreDraw(dxCommon->GetCommandList());

	if (textModel_) {
		textModel_->Draw(textTransform_, *camera_);
	}

	Model::PostDraw();
	if (fade_)
		fade_->Draw();
}

GameClearScene::~GameClearScene() {
	delete textModel_;
	delete camera_;
	delete fade_;
}
