#include "SoundManager.h"

using namespace KamataEngine;

void SoundManager::Initialize() {
	// 一括読み込み
	handleTitle_ = Audio::GetInstance()->LoadWave("451_BPM190.mp3");
	handleGame_ = Audio::GetInstance()->LoadWave("160_BPM172.mp3");
	handleClear_ = Audio::GetInstance()->LoadWave("126_BPM142.mp3");
	handleOver_ = Audio::GetInstance()->LoadWave("354_BPM180.mp3");

	// 音量設定
	float vol = 0.3f;
	Audio::GetInstance()->SetVolume(handleTitle_, vol);
	Audio::GetInstance()->SetVolume(handleGame_, vol);
	Audio::GetInstance()->SetVolume(handleClear_, vol);
	Audio::GetInstance()->SetVolume(handleOver_, vol);
}

void SoundManager::ChangeBgm(SceneType scene) {
	int nextResourceHandle = -1;
	switch (scene) {
	case SceneType::Title:
		nextResourceHandle = handleTitle_;
		break;
	case SceneType::Game:
		nextResourceHandle = handleGame_;
		break;
	case SceneType::Clear:
		nextResourceHandle = handleClear_;
		break;
	case SceneType::GameOver:
		nextResourceHandle = handleOver_;
		break;
	}

	if (currentBgmHandle_ != -1) {
		Audio::GetInstance()->StopWave(currentBgmHandle_);
		currentBgmHandle_ = -1;
	}

	if (nextResourceHandle != -1) {
		currentBgmHandle_ = Audio::GetInstance()->PlayWave(nextResourceHandle, true);
	}

}

void SoundManager::Finalize() {
	// 音を止める
	if (currentBgmHandle_ != -1) {
		Audio::GetInstance()->StopWave(currentBgmHandle_);
	}
}