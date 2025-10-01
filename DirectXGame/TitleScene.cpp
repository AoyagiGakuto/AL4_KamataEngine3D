#include "TitleScene.h"
#include "MyMath.h"
#include <cmath>
#include <numbers>

TitleScene::TitleScene() {}

TitleScene::~TitleScene() {
	delete titleFontModel_;
	delete backgroundModel_;
	delete skyDomeModel_;
	delete pressSpaceModel_;
	delete tutorialGuideModel_;
	delete camera_;
	delete fade_;
}

void TitleScene::Initialize() {
	// ==== タイトルロゴ ====
	titleFontModel_ = Model::CreateFromOBJ("rittaitaiketu");
	titleTransform_.Initialize();
	titleTransform_.translation_ = {0.0f, 5.0f, 6.0f};
	titleTransform_.scale_ = {2.0f, 2.0f, 2.0f};

	// ==== 背景OBJ ====
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

	// ==== PressSpace ====
	pressSpaceModel_ = Model::CreateFromOBJ("PressSpace");
	pressSpaceTransform_.Initialize();
	pressSpaceTransform_.translation_ = {0.0f, -3.0f, 6.0f};
	pressSpaceTransform_.scale_ = {3.5f, 3.5f, 1.5f};

	// ==== Tutorial 誘導OBJ ====
	tutorialGuideModel_ = Model::CreateFromOBJ("TutorialGuide");
	tutorialGuideTransform_.Initialize();
	tutorialGuideTransform_.translation_ = {-1.0f, -6.0f, 6.0f};
	tutorialGuideTransform_.scale_ = {0.5f, 0.5f, 2.0f};

	// ==== カメラ ====
	camera_ = new Camera();
	camera_->Initialize();
	camera_->translation_ = {0.0f, 0.0f, -15.0f};
	camera_->TransferMatrix();

	// ==== フェード ====
	fade_ = new Fade();
	fade_->Initialize();
	phase_ = Phase::kFadeIn;
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	// 状態
	blinkTimer_ = 0.0f;
	blinkVisible_ = true;
	logoMoveTimer_ = 0.0f;
	finished_ = false;
	nextAction_ = NextAction::StartGame; // 既定はSpaceでゲーム開始
}

void TitleScene::Update() {
	switch (phase_) {
	case Phase::kFadeIn:
		fade_->Update();
		if (fade_->IsFinished())
			phase_ = Phase::kMain;
		break;

	case Phase::kMain:
		// ★ Space ならゲーム開始
		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			nextAction_ = NextAction::StartGame;
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, 1.0f);
		}
		// ★ T ならチュートリアルへ
		else if (Input::GetInstance()->TriggerKey(DIK_T)) {
			nextAction_ = NextAction::OpenTutorial;
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, 1.0f);
		}
		break;

	case Phase::kFadeOut:
		fade_->Update();
		if (fade_->IsFinished()) {
			finished_ = true;
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

	// 天球をカメラに追従
	skyDomeWT_.translation_ = camera_->translation_;
	skyDomeWT_.matWorld_ = MakeAffineMatrix(skyDomeWT_.scale_, skyDomeWT_.rotation_, skyDomeWT_.translation_);
	skyDomeWT_.TransferMatrix();

	// 行列更新
	titleTransform_.matWorld_ = MakeAffineMatrix(titleTransform_.scale_, titleTransform_.rotation_, titleTransform_.translation_);
	titleTransform_.TransferMatrix();

	backgroundTransform_.matWorld_ = MakeAffineMatrix(backgroundTransform_.scale_, backgroundTransform_.rotation_, backgroundTransform_.translation_);
	backgroundTransform_.TransferMatrix();

	pressSpaceTransform_.matWorld_ = MakeAffineMatrix(pressSpaceTransform_.scale_, pressSpaceTransform_.rotation_, pressSpaceTransform_.translation_);
	pressSpaceTransform_.TransferMatrix();

	tutorialGuideTransform_.matWorld_ = MakeAffineMatrix(tutorialGuideTransform_.scale_, tutorialGuideTransform_.rotation_, tutorialGuideTransform_.translation_);
	tutorialGuideTransform_.TransferMatrix();
}

void TitleScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	Model::PreDraw(dxCommon->GetCommandList());

	// 1) 天球
	if (skyDomeModel_)
		skyDomeModel_->Draw(skyDomeWT_, *camera_);
	// 2) 任意の平面背景
	if (backgroundModel_)
		backgroundModel_->Draw(backgroundTransform_, *camera_);
	// 3) ロゴ・誘導・PressSpace
	if (titleFontModel_)
		titleFontModel_->Draw(titleTransform_, *camera_);
	if (tutorialGuideModel_)
		tutorialGuideModel_->Draw(tutorialGuideTransform_, *camera_);
	if (pressSpaceModel_ && blinkVisible_)
		pressSpaceModel_->Draw(pressSpaceTransform_, *camera_);

	Model::PostDraw();

	// 4) フェード
	if (fade_)
		fade_->Draw();
}
