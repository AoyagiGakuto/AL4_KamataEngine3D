#pragma once
#include "KamataEngine.h"

// シーンの種類をここで使うので定義
enum class SceneType { Title, Tutorial, Game, Clear, GameOver };

class SoundManager {
public:
	// 初期化
	void Initialize();

	// シーンに合わせてBGMを替える
	void ChangeBgm(SceneType scene);

	void Finalize();

private:
	// 今流れているBGM
	int currentBgmHandle_ = -1;

	// 各シーンのBGM
	int handleTitle_ = -1;
	int handleGame_ = -1;
	int handleClear_ = -1;
	int handleOver_ = -1;
};