#include "Logger.h"
#include <ctime>
#include <chrono>
#include <sstream>
#include <iomanip>
using logging::Logger;
using logging::LogLevel;

std::string Logger::levelToString(LogLevel level) const {
	switch (level) {
	case LogLevel::DEBUG: return "DEBUG";
	case LogLevel::INFO: return "INFO";
	case LogLevel::WARNING: return "WARNING";
	case LogLevel::ERR: return "ERROR";
	default: return "UNKNOWN";
	}
}

std::string Logger::getCurrentTimestamp() const {
	auto now = std::chrono::system_clock::now();
	std::string formatted_time = std::format("{0:%F_%T}", now);
	return formatted_time;
}

void Logger::SetLogFile(const std::string& filename) {
	std::lock_guard<std::mutex> lock(logMutex);
	if (logFile.is_open()) {
		logFile.close();
	}
	logFile.open(filename, std::ios::app);
}

void Logger::Log(LogLevel level, const std::string& message, const char* filename, const int line) {
#ifdef DISABLE_LOGGING
	return;
#endif
	if (level < minLogLevel) {
		return;
	}
	std::string currentTimestamp = getCurrentTimestamp();
	std::lock_guard<std::mutex> lock(logMutex);
	std::stringstream ss;
	ss << "[" << currentTimestamp << "] "
		<< "[" << levelToString(level) << "] "
		<< "[" << filename << ":" << line << "] "
		<< message << std::endl;
	std::string formattedMessage = ss.str();

	std::cout << formattedMessage;
	if (logFile.is_open()) {
		logFile << formattedMessage;
		logFile.flush();
	}
}