#include "TitleScene.h"
#include "MyMath.h"
#include <cmath>

TitleScene::TitleScene() {}

TitleScene::~TitleScene() {
	delete titleFontModel_;
	delete playerModel_;
	delete camera_;
	delete fade_;
}

void TitleScene::Initialize() {
	// タイトルロゴ
	titleFontModel_ = Model::CreateFromOBJ("titleFont");
	titleTransform_.Initialize();
	titleTransform_.translation_ = {0.0f, 1.0f, 0.0f};
	titleTransform_.scale_ = {2.0f, 2.0f, 2.0f};

	// 画面下にプレイヤーモデルを置いて装飾
	playerModel_ = Model::CreateFromOBJ("player");
	playerTransform_.Initialize();
	playerTransform_.translation_ = {0.0f, -2.0f, 0.0f};
	playerTransform_.scale_ = {10.0f, 10.0f, 10.0f};
	playerTransform_.rotation_ = {0.0f, 3.14f, 0.0f};

	// カメラ
	camera_ = new Camera();
	camera_->Initialize();

	// フェード（シーン内）
	fade_ = new Fade();
	fade_->Initialize();
	phase_ = Phase::kFadeIn;
	fade_->Start(Fade::Status::FadeIn, 1.0f); // 起動時にフェードイン

	// 状態初期化
	blinkTimer_ = 0.0f;
	blinkVisible_ = true;
	logoMoveTimer_ = 0.0f;
	finished_ = false;
}

void TitleScene::Update() {
	switch (phase_) {
	case Phase::kFadeIn:
		fade_->Update();
		if (fade_->IsFinished())
			phase_ = Phase::kMain;
		break;

	case Phase::kMain:
		// Space でゲームへ → フェードアウト開始
		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, 1.0f);
		}
		break;

	case Phase::kFadeOut:
		fade_->Update();
		if (fade_->IsFinished()) {
			finished_ = true; // main がこれを見て GameScene に切替
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
}

void TitleScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	Model::PreDraw(dxCommon->GetCommandList());

	titleFontModel_->Draw(titleTransform_, *camera_);
	playerModel_->Draw(playerTransform_, *camera_);

	Model::PostDraw();

	// ★最後にフェードを被せる（常に最後！）
	if (fade_)
		fade_->Draw();
}
