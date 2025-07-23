#include "TitleScene.h"
#include <cmath>
#include "MyMath.h"

TitleScene::TitleScene() {}

TitleScene::~TitleScene() {
	delete titleFontModel_;
	delete playerModel_;
	delete camera_;
}

void TitleScene::Initialize() {
	// タイトルロゴ
	titleFontModel_ = Model::CreateFromOBJ("titleFont");
	titleTransform_.Initialize();
	titleTransform_.translation_ = {0.0f, 1.0f, 0.0f};
	titleTransform_.scale_ = {2.0f, 2.0f, 2.0f};

	// プレイヤーOBJ（画面下に置く）
	playerModel_ = Model::CreateFromOBJ("player");
	playerTransform_.Initialize();
	playerTransform_.translation_ = {0.0f, -2.0f, 0.0f};
	playerTransform_.scale_ = {10.0f, 10.0f, 10.0f};
	playerTransform_.rotation_ = {0.0f, 3.14f, 0.0f};

	// カメラ
	camera_ = new Camera();
	camera_->Initialize();

	blinkTimer_ = 0.0f;
	blinkVisible_ = true;
	finished_ = false;

	logoMoveTimer_ = 0.0f;
}

void TitleScene::Update() {
	// Spaceでシーン終了
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		finished_ = true;
	}

	// 点滅演出
	blinkTimer_ += 1.0f / 60.0f;
	if (blinkTimer_ > 0.5f) {
		blinkVisible_ = !blinkVisible_;
		blinkTimer_ = 0.0f;
	}

	// ★タイトルロゴ上下にふわふわ揺れる
	logoMoveTimer_ += 1.0f / 60.0f;
	float baseY = 10.0f;                                     // ロゴの基準位置をさらに上へ
	float offsetY = std::sin(logoMoveTimer_ * 2.0f) * 0.2f; // ±0.2の上下

	// ✅ baseY + offsetY でちゃんと上に配置される
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

	// タイトルロゴ描画（上下揺れ）
	titleFontModel_->Draw(titleTransform_, *camera_);

	// プレイヤーを画面下に正面向きで表示
	playerModel_->Draw(playerTransform_, *camera_);

	Model::PostDraw();
}
