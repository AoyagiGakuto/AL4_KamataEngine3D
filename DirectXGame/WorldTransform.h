/* #pragma once
#include "MyMath.h"
#include "MapChipField.h"

class WorldTransform {
public:
	Vector3 translation_{0, 0, 0};
	Vector3 rotation_{0, 0, 0};
	Vector3 scale_{1, 1, 1};

	Matrix4 matWorld_{};

	void Initialize() {
		translation_ = {0, 0, 0};
		rotation_ = {0, 0, 0};
		scale_ = {1, 1, 1};
		TransferMatrix();
	}

	void TransferMatrix() {
		// 簡易的なアフィン変換行列を作成
		matWorld_ = MakeAffineMatrix(scale_, rotation_, translation_);
	}
};
*/