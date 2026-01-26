#pragma once
#include "KamataEngine.h"

struct AABB {
	KamataEngine::Vector3 min; // 最小座標
	KamataEngine::Vector3 max; // 最大座標
};

float EaseInOut(float x1, float x2, float t);

KamataEngine::Matrix4x4 MakeAffineMatrix(const KamataEngine::Vector3& scale, const KamataEngine::Vector3& rotation, const KamataEngine::Vector3& translation);