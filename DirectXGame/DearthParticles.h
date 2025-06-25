#pragma once  
#include "Player.h"  
#include "MapChipField.h"  
#include "MyMath.h"  
#include <algorithm>  
#include <array>  
#include <numbers>  

struct Particle {  
	WorldTransform transform;  
	Vector3 velocity;  
	float life; // 残り寿命  
};  

class DearthParticles {  
public:  
	void Initialize(Model* model);  
	void Update();  
	void Draw();  

	// 8方向にパーティクルを生成  
	void Emit8Directions(const Vector3& position, float speed, float life);  

private:  
	std::vector<Particle> particles_;
	Camera* camera_ = nullptr;
	Model* model_ = nullptr;  
	DearthParticles* dearthParticles_ = nullptr;  
};