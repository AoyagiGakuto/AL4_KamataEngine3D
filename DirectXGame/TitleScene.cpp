#include "TitleScene.h"
#include <cmath>
#include "MyMath.h"

TitleScene::TitleScene() {}

TitleScene::~TitleScene() {
	delete titleFontModel_;
	delete camera_;
}

void TitleScene::Initialize() {
	// タイトルロゴ読み込み（OBJは "titleFont.obj"）
	titleFontModel_ = Model::CreateFromOBJ("titleFont");

	// タイトルロゴ位置調整
	titleTransform_.Initialize();
	titleTransform_.translation_ = {0.0f, 0.0f, 0.0f};
	titleTransform_.scale_ = {1.0f, 1.0f, 1.0f};

	// カメラ初期化
	camera_ = new Camera();
	camera_->Initialize();

	blinkTimer_ = 0.0f;
	blinkVisible_ = true;
	finished_ = false;
}

void TitleScene::Update() {
	// Spaceで終了フラグON
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		finished_ = true;
	}

	// 点滅演出
	blinkTimer_ += 1.0f / 60.0f;
	if (blinkTimer_ > 0.5f) {
		blinkVisible_ = !blinkVisible_;
		blinkTimer_ = 0.0f;
	}

	titleTransform_.matWorld_ = MakeAffineMatrix(titleTransform_.scale_, titleTransform_.rotation_, titleTransform_.translation_);
	titleTransform_.TransferMatrix();
}

void TitleScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Model::PreDraw(dxCommon->GetCommandList());

	// タイトルロゴ表示
	titleFontModel_->Draw(titleTransform_, *camera_);

	// blinkVisible_ を使って PressSpace的な演出追加も可

	Model::PostDraw();
}
