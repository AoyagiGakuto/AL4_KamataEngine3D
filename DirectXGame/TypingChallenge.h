#pragma once
#include "KamataEngine.h"
#include <algorithm>
#include <cctype>
#include <string>
#include <vector>

using namespace KamataEngine;

class TypingChallenge {
public:
	void Initialize(float timeLimitSec = 10.0f) {
		timeLimit_ = timeLimitSec;
		Reset();
	}

	void Start(const std::string& word, float timeLimitSec = -1.0f) {
		active_ = true;
		target_ = word;
		typed_.clear();
		timer_ = 0.0f;
		success_ = false;
		timeout_ = false;
		if (timeLimitSec > 0.0f)
			timeLimit_ = timeLimitSec;
	}

	void Cancel() { Reset(); }

	// キー入力を拾って進行
	void Update() {
		if (!active_)
			return;

		timer_ += 1.0f / 60.0f;
		if (timer_ >= timeLimit_) {
			timeout_ = true;
			active_ = false;
			return;
		}

		Input* in = Input::GetInstance();

		// ---- ここがポイント：DirectInputのスキャンコードを配列で明示 ----
		static const unsigned char kDIKLetters[26] = {DIK_A, DIK_B, DIK_C, DIK_D, DIK_E, DIK_F, DIK_G, DIK_H, DIK_I, DIK_J, DIK_K, DIK_L, DIK_M,
		                                              DIK_N, DIK_O, DIK_P, DIK_Q, DIK_R, DIK_S, DIK_T, DIK_U, DIK_V, DIK_W, DIK_X, DIK_Y, DIK_Z};
		static const unsigned char kDIKDigits[10] = {DIK_0, DIK_1, DIK_2, DIK_3, DIK_4, DIK_5, DIK_6, DIK_7, DIK_8, DIK_9};

		// A〜Z
		for (int i = 0; i < 26; ++i) {
			if (in->TriggerKey(kDIKLetters[i])) {
				char c = static_cast<char>('a' + i);
				AppendChar(c);
			}
		}
		// 0〜9（数字行）
		for (int i = 0; i < 10; ++i) {
			if (in->TriggerKey(kDIKDigits[i])) {
				char c = static_cast<char>('0' + i);
				AppendChar(c);
			}
		}

		// スペース / バックスペース / エンター
		if (in->TriggerKey(DIK_SPACE))
			AppendChar(' ');
		if (in->TriggerKey(DIK_BACK)) {
			if (!typed_.empty())
				typed_.pop_back();
		}
		if (in->TriggerKey(DIK_RETURN)) {
			if (typed_ == target_) {
				success_ = true;
				active_ = false;
			}
		}

		// 自動判定（全部打ったら即チェック）
		if (typed_.size() == target_.size()) {
			if (typed_ == target_) {
				success_ = true;
				active_ = false;
			}
		}
	}

	bool IsActive() const { return active_; }
	bool IsSuccess() const { return success_; }
	bool IsTimeout() const { return timeout_; }

	float GetRemainTime() const { return (std::max)(0.0f, timeLimit_ - timer_); }
	const std::string& GetTarget() const { return target_; }
	const std::string& GetTyped() const { return typed_; }

	void DrawStub() const {}

private:
	void Reset() {
		active_ = false;
		success_ = false;
		timeout_ = false;
		target_.clear();
		typed_.clear();
		timer_ = 0.0f;
	}

	void AppendChar(char c) {
		// 小文字に統一
		if (std::isupper(static_cast<unsigned char>(c))) {
			c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
		}
		if (typed_.size() < target_.size()) {
			typed_.push_back(c);
		}
	}

	bool active_ = false;
	bool success_ = false;
	bool timeout_ = false;

	std::string target_;
	std::string typed_;
	float timeLimit_ = 10.0f;
	float timer_ = 0.0f;
};
