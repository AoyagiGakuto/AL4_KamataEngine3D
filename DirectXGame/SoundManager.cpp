#include "SoundManager.h"

using namespace KamataEngine;

void SoundManager::Initialize() {
	// 一括読み込み
	handleTitle_ = Audio::GetInstance()->LoadWave("Drumnbass_02.mp3");
	handleGame_ = Audio::GetInstance()->LoadWave("kakushin.mp3");
	handleClear_ = Audio::GetInstance()->LoadWave("Clear.mp3");
	handleOver_ = Audio::GetInstance()->LoadWave("GameOver.mp3");

	// 音量設定
	float vol = 0.5f;
	Audio::GetInstance()->SetVolume(handleTitle_, vol);
	Audio::GetInstance()->SetVolume(handleGame_, vol);
	Audio::GetInstance()->SetVolume(handleClear_, vol);
	Audio::GetInstance()->SetVolume(handleOver_, vol);
}

void SoundManager::ChangeBgm(SceneType scene) {
	// 今流れている音を止める
	if (currentBgmHandle_ != -1) {
		Audio::GetInstance()->StopWave(currentBgmHandle_);
	}

	// 次に流す音を決める
	int nextHandle = -1;
	switch (scene) {
	case SceneType::Title:
		nextHandle = handleTitle_;
		break;
	case SceneType::Game:
	case SceneType::Tutorial:
		nextHandle = handleGame_;
		break;
	case SceneType::Clear:
		nextHandle = handleClear_;
		break;
	case SceneType::GameOver:
		nextHandle = handleOver_;
		break;
	}

	// 再生する
	if (nextHandle != -1) {
		if (currentBgmHandle_ != nextHandle) {
			Audio::GetInstance()->PlayWave(nextHandle, true);
			currentBgmHandle_ = nextHandle;
		}
	} else {
		currentBgmHandle_ = -1;
	}
}

void SoundManager::Finalize() {
	// 音を止める
	if (currentBgmHandle_ != -1) {
		Audio::GetInstance()->StopWave(currentBgmHandle_);
	}
}