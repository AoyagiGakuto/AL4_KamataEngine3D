#include "Skydome.h"

void Skydome::Initialize() {

}

void Skydome::Update() {}

void Skydome::Draw() {
	// 3Dオブジェクトの描画
	model_->Draw(worldTransform_, *camera_);
}
