#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

struct AABB {
	KamataEngine::Vector3 min; // 最小座標
	KamataEngine::Vector3 max; // 最大座標
};

float EaseInOut(float x1, float x2, float t);

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotation, const Vector3& translation);