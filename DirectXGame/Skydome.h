#pragma once
#include "KamataEngine.h"
#include <vector>

class Skydome {
public:
	//void Draw();

private:
	// ワールド
	KamataEngine::WorldTransform worldTransform_;
	// model
	KamataEngine::Model* model_ = nullptr;
	// カメラ
	KamataEngine::Camera* camera_ = nullptr;
};
