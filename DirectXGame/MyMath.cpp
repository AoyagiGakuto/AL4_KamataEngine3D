#include "MyMath.h"

#include <cmath>
#include <numbers>

using namespace KamataEngine;
using namespace MathUtility;

float EaseInOut(float x1, float x2, float t) {
	float easedT = (std::cosf(std::numbers::pi_v<float>*t)-1.0f) /2.0f;
	return Lerp(x1,x2,easedT);
}

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotation, const Vector3& translation) {
	// スケーリング行列
	Matrix4x4 matScale = MakeScaleMatrix(scale);

	// 回転行列
	Matrix4x4 matRotX = MakeRotateXMatrix(rotation.x);
	Matrix4x4 matRotY = MakeRotateYMatrix(rotation.y);
	Matrix4x4 matRotZ = MakeRotateZMatrix(rotation.z);
	Matrix4x4 matRot = matRotZ * matRotY * matRotX;

	// 平行移動行列
	Matrix4x4 matTrans = MakeTranslateMatrix(translation);

	// スケーリング、回転、平行移動
	Matrix4x4 matWorld = matScale * matRot * matTrans;
	return matWorld;
}
