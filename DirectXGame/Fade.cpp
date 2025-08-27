#include "Fade.h"
#include <algorithm>
using namespace KamataEngine;

// 画面サイズに合わせるヘルパ
static inline void ResizeToBackbuffer(KamataEngine::Sprite* spr) {
	auto* dx = DirectXCommon::GetInstance();
	spr->SetSize(Vector2(static_cast<float>(dx->GetBackBufferWidth()), static_cast<float>(dx->GetBackBufferHeight())));
	spr->SetPosition(Vector2(0.0f, 0.0f));
}

void Fade::Initialize() {
	textureHandle = TextureManager::Load("white1x1.png");
	sprite_ = Sprite::Create(textureHandle, {0, 0});
	sprite_->SetColor(Vector4(0, 0, 0, 1)); // 黒
	ResizeToBackbuffer(sprite_);            // 初期フレームから全面
}

void Fade::Update() {
	switch (status_) {
	case Status::None:
		break;
	case Status::FadeIn:
		counter_ += 1.0f / 60.0f;
		sprite_->SetColor(Vector4(0, 0, 0, 1.0f - std::clamp(counter_ / duration_, 0.0f, 1.0f)));
		if (counter_ >= duration_) {
			counter_ = duration_;
			status_ = Status::None;
		}
		break;
	case Status::FadeOut:
		counter_ += 1.0f / 60.0f;
		sprite_->SetColor(Vector4(0, 0, 0, std::clamp(counter_ / duration_, 0.0f, 1.0f)));
		if (counter_ >= duration_) {
			counter_ = duration_;
			status_ = Status::None;
		}
		break;
	}
	ResizeToBackbuffer(sprite_);
}

void Fade::Draw() {
	if (status_ == Status::None)
		return;
	auto* dx = DirectXCommon::GetInstance();
	Sprite::PreDraw(dx->GetCommandList());
	sprite_->Draw();
	Sprite::PostDraw();
}

void Fade::Start(Status status, float duration) {
	status_ = status;
	duration_ = duration;
	counter_ = 0.0f;
	ResizeToBackbuffer(sprite_);
	sprite_->SetColor(
	    status_ == Status::FadeOut ? Vector4(0, 0, 0, 0.0f)   // 透明→黒へ
	                               : Vector4(0, 0, 0, 1.0f)); // 黒→透明へ
}

void Fade::Stop() { status_ = Status::None; }
bool Fade::IsFinished() const { return status_ == Status::None; }
