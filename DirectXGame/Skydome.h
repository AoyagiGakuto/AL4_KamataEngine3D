#pragma once
#include "KamataEngine.h"
#include <vector>

using namespace KamataEngine;

class Skydome {
public:
	void Initialize();
	void Update();
	void Draw();

private:
	// ワールド
	WorldTransform worldTransform_;
	// model
	Model* model_ = nullptr;
};
