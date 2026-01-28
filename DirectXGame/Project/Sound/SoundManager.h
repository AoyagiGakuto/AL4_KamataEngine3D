#pragma once
#include "KamataEngine.h"

// シーンの種類
enum class SceneType { Title, Game, Clear, GameOver };

// 効果音の種類
enum class SeType {
	Punch, // 近接攻撃
	Bomb,  // 弾
	Dash,  // 高速移動
	Slash  // R攻撃の斬撃
};

class SoundManager {
public:
	// 初期化
	void Initialize();

	// シーンに合わせてBGMを替える
	void ChangeBgm(SceneType scene);

	// 効果音再生
	void PlaySe(SeType type);

	void Finalize();

private:
	// 今流れているBGM
	int currentBgmHandle_ = -1;

	// 各シーンのBGM
	int handleTitle_ = -1;
	int handleGame_ = -1;
	int handleClear_ = -1;
	int handleOver_ = -1;

	// 効果音
	int handleSePunch_ = -1;
	int handleSeBomb_ = -1;
	int handleSeDash_ = -1;
	int handleSeSlash_ = -1;
};