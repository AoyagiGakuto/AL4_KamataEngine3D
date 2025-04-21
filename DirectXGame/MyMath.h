#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

class MyMath {
	Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotation, const Vector3& translation);
};
