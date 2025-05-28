#pragma once
#include "KamataEngine.h"

class CameraController {
public:
	void Initialize();
	void Update();

private:
	// カメラ
	KamataEngine::Camera camera_;
};
