#include "MyMath.h"

using namespace KamataEngine;
using namespace MathUtility;

Matrix4x4 MyMath::MakeAffineMatrix(const Vector3& scale, const Vector3& rotation, const Vector3& translation) 
{ 
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
	Matrix4x4 matWorld = matTrans * matRot * matScale;

	return matWorld;
}
