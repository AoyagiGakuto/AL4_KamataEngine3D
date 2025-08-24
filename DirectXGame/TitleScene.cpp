#include "TitleScene.h"
#include "MyMath.h"
#include <cmath>

TitleScene::TitleScene() {}

TitleScene::~TitleScene() {
	delete titleFontModel_;
	delete backgroundModel_;
	delete pressSpaceModel_;
	delete camera_;
	delete fade_;
}

void TitleScene::Initialize() {
	// タイトルロゴ
	titleFontModel_ = Model::CreateFromOBJ("rittaitaiketu");
	titleTransform_.Initialize();
	titleTransform_.translation_ = {0.0f, 5.0f, 0.0f};
	titleTransform_.scale_ = {2.0f, 2.0f, 2.0f};

	// ★背景
	backgroundModel_ = Model::CreateFromOBJ("background");
	backgroundTransform_.Initialize();
	// 画面いっぱいに見せたい想定のサイズ（必調整）
	backgroundTransform_.scale_ = {100.0f, 100.0f, 10.0f};

	// ★PressSpace（点滅表示）
	pressSpaceModel_ = Model::CreateFromOBJ("PressSpace");
	pressSpaceTransform_.Initialize();
	pressSpaceTransform_.translation_ = {0.0f, -3.0f, 0.0f};
	pressSpaceTransform_.scale_ = {3.5f, 3.5f, 1.5f};

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

	// 「PressSpace」点滅
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

	backgroundTransform_.matWorld_ = MakeAffineMatrix(backgroundTransform_.scale_, backgroundTransform_.rotation_, backgroundTransform_.translation_);
	backgroundTransform_.TransferMatrix();

	pressSpaceTransform_.matWorld_ = MakeAffineMatrix(pressSpaceTransform_.scale_, pressSpaceTransform_.rotation_, pressSpaceTransform_.translation_);
	pressSpaceTransform_.TransferMatrix();
}

void TitleScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	Model::PreDraw(dxCommon->GetCommandList());

	// 背景 → ロゴ → PressSpace（点滅）
	if (backgroundModel_)
		backgroundModel_->Draw(backgroundTransform_, *camera_);
	
	if (titleFontModel_)
		titleFontModel_->Draw(titleTransform_, *camera_);
	if (pressSpaceModel_ && blinkVisible_)
		pressSpaceModel_->Draw(pressSpaceTransform_, *camera_);

	Model::PostDraw();

	// 最後にフェードを被せる
	if (fade_)
		fade_->Draw();
}
