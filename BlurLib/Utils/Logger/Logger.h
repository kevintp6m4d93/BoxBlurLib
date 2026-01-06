#pragma once
#include <mutex>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

namespace logging {
	enum class LogLevel {
		DEBUG = 0,
		INFO = 1,
		WARNING = 2,
		ERR = 3
	};

	class Logger {
	public:
		static Logger& GetInstance() {
			static Logger instance;
			return instance;
		}
		void SetMinLogLevel(LogLevel level) {
			minLogLevel = level;
		}
		void Log(LogLevel level, const std::string& message, const char* filename, const int line);
		void SetLogFile(const std::string& filename);

	private:
		std::ofstream logFile;
		std::mutex logMutex;
		LogLevel minLogLevel;

		std::string getCurrentTimestamp() const;
		std::string levelToString(LogLevel level) const;
		Logger() : minLogLevel(LogLevel::DEBUG) {}
		~Logger() {
			logFile.close();
		}
		Logger(const Logger&) = delete;
		void operator=(const Logger&) = delete;
	};
}

#ifdef DISABLE_LOGGING
#define LOG_INTERNAL(logLevel, msg, file, line) ((void)0)
#else
#define LOG_INTERNAL(logLevel, msg, file, line) logging::Logger::GetInstance().Log(logLevel, msg, file, line)
#endif

#define LOG(logLevel, msg) LOG_INTERNAL(logLevel, msg, __FILE__, __LINE__)
#define LOG_DEBUG(msg) LOG_INTERNAL(logging::LogLevel::DEBUG, msg, __FILE__, __LINE__)
#define LOG_INFO(msg) LOG_INTERNAL(logging::LogLevel::INFO, msg, __FILE__, __LINE__)
#define LOG_WARNING(msg) LOG_INTERNAL(logging::LogLevel::WARNING, msg, __FILE__, __LINE__)
#define LOG_ERROR(msg) LOG_INTERNAL(logging::LogLevel::ERR, msg, __FILE__, __LINE__)