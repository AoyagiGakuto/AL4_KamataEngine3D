#pragma once
#include "KamataEngine.h"

class Fade {
public:
	enum class Status {
		None,
		FadeIn,
		FadeOut,
	};

	// 初期化
	void Initialize();
	
	// 更新
	void Update();
	
	// 描画
	void Draw();
	
	// フェード開始
	void Start(Status status, float duration);
	
	// フェード停止
	void Stop();

	// フェード終了確認
	bool IsFinished() const;

	// フェード状態取得
	Status GetStatus() const { return status_; }

private:

	/*
	// --- リソース ---
	*/

	uint32_t textureHandle = 0;
	KamataEngine::Sprite* sprite_ = nullptr;
	
	/*
	// --- 状態管理 ---
	*/

	Status status_ = Status::None;

	/*
	// --- 状態管理 ---
	*/

	// 指定された期間
	float duration_ = 0.0f;
	// 経過時間カウンタ
	float counter_ = 0.0f;
};
