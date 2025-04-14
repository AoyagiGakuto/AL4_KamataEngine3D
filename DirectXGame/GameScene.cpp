#include "GameScene.h"

using namespace KamataEngine;

//========================================
// 初期化処理
//========================================

void GameScene::Initialize() {
	// 初期化処理

	// テクスチャの読み込み
	textureHandle_ = TextureManager::Load("white1x1.png");
	sprite_ = Sprite::Create(textureHandle_, {100, 50});
}

//========================================
// 更新処理
//========================================

void GameScene::Update() {
	// 更新処理

	// スプライトの今の座標を取得
	Vector2 position = sprite_->GetPosition();
	// 座標{2,1}に移動
	position.x += 2.0f;
	position.y += 1.0f;
	// 移動した座標をスプライトに反映
	sprite_->SetPosition(position);
}

//========================================
// 描画処理
//========================================

void GameScene::Draw() {
	// 描画処理

	// DirectXCommonのインスタンスを取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// スプライト描画前処理
	Sprite::PreDraw(dxCommon->GetCommandList());

	// ここにスプライトインスタンスの描画処理を記述する
	sprite_->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();
}

