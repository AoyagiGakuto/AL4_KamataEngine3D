#pragma once
#include "KamataEngine.h"

class GameScene {
public:

	//========================================
	// 初期化処理
	//========================================

	// 初期化
	void Initialize();

private:

	// テクスチャハンドル
	uint32_t textureHandle_ = 0;

public:

	// スプライト
	KamataEngine::Sprite* sprite_ = nullptr;

	//========================================
	// 更新処理
	//========================================

	// 更新
	void Update();

	//========================================
	// 描画処理
	//========================================

	// 描画
	void Draw();

	// デストラクタ
	~GameScene() {
		// スプライトの解放
		delete sprite_;
	}
};
