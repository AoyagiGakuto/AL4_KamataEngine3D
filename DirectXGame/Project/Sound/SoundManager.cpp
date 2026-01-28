#include "SoundManager.h"

using namespace KamataEngine;

void SoundManager::Initialize() {
	// bgm読み込み
	handleTitle_ = Audio::GetInstance()->LoadWave("451_BPM190.mp3");
	handleGame_ = Audio::GetInstance()->LoadWave("160_BPM172.mp3");
	handleClear_ = Audio::GetInstance()->LoadWave("126_BPM142.mp3");
	handleOver_ = Audio::GetInstance()->LoadWave("354_BPM180.mp3");

	// 効果音読み込み
	handleSePunch_ = Audio::GetInstance()->LoadWave("punch.mp3");
	handleSeBomb_ = Audio::GetInstance()->LoadWave("bomb.mp3");
	handleSeDash_ = Audio::GetInstance()->LoadWave("kousoku.mp3");
	handleSeSlash_ = Audio::GetInstance()->LoadWave("Rsound.mp3");
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

void SoundManager::PlaySe(SeType type) {
	int handle = -1;
	switch (type) {
	case SeType::Punch:
		handle = handleSePunch_;
		break;
	case SeType::Bomb:
		handle = handleSeBomb_;
		break;
	case SeType::Dash:
		handle = handleSeDash_;
		break;
	case SeType::Slash:
		handle = handleSeSlash_;
		break;
	}

	if (handle != -1) {
		Audio::GetInstance()->PlayWave(handle);
	}
}

void SoundManager::Finalize() {
	// 今流れてる音を止める
	if (currentBgmHandle_ != -1) {
		Audio::GetInstance()->StopWave(currentBgmHandle_);
	}
}