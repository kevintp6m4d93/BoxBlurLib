#pragma once
#include <functional>
#include <chrono>

class ScopeTimer {
public:
	ScopeTimer(std::function<void(double)> loggingCallback)
		: loggingCallback_(loggingCallback), startTime_(std::chrono::steady_clock::now()) {
	}
	~ScopeTimer() {
		auto endTime = std::chrono::steady_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime_).count();
		if (loggingCallback_) {
			loggingCallback_(duration);
		}
	}
private:
	std::function<void(double)> loggingCallback_;
	std::chrono::steady_clock::time_point startTime_;
};