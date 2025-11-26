#pragma once
#include "KamataEngine.h"
#include "MyMath.h"
#include <array>
#include <string>
#include <vector>

using namespace KamataEngine;

class HitEffect {
public:
	~HitEffect();

	void Initialize(std::array<Model*, 10>& numberModels, Camera* camera, const Vector3& pos, int number);

	void Update();
	void Draw();
	bool IsAlive() const { return alive_; }

	void UpdatePosition(const Vector3& basePos);

private:
	std::vector<WorldTransform*> transforms_;

	std::vector<Model*> modelsToDraw_;
	Camera* camera_ = nullptr;

	float speed_ = 0.02f;
	float lifetime_ = 0.75f;
	float maxLifetime_ = 0.75f;
	bool alive_ = true;

	static inline const float kDigitWidth = 0.4f;

	float scale_ = 0.5f;
	std::string numStr_ = "";
	float yOffset_ = 0.0f;
	Vector3 basePos_ = {};
	ObjectColor objectColor_;
	Vector4 color_ = {1.0f, 1.0f, 1.0f, 1.0f};
};