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

		for (int i = 0; i < 26; ++i) {
			int dik = DIK_A + i;
			if (in->TriggerKey(static_cast<unsigned char>(dik))) {
				char c = static_cast<char>('a' + i);
				AppendChar(c);
			}
		}
		for (int i = 0; i < 10; ++i) {
			int dik = DIK_0 + i;
			if (in->TriggerKey(static_cast<unsigned char>(dik))) {
				char c = static_cast<char>('0' + i);
				AppendChar(c);
			}
		}
		if (in->TriggerKey(static_cast<unsigned char>(DIK_SPACE))) {
			AppendChar(' ');
		}
		if (in->TriggerKey(static_cast<unsigned char>(DIK_BACK))) {
			if (!typed_.empty())
				typed_.pop_back();
		}
		if (in->TriggerKey(static_cast<unsigned char>(DIK_RETURN))) {
			if (typed_ == target_) {
				success_ = true;
				active_ = false;
			}
		}
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

	float RemainingTime() const { return (std::max)(0.0f, timeLimit_ - timer_); }
	const std::string& Target() const { return target_; }
	const std::string& TypedText() const { return typed_; }

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
