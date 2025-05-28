#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

float EaseInOut(float x1, float x2, float t);

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotation, const Vector3& translation);
