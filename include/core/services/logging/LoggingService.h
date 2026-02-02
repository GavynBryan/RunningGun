#pragma once

#include <core/framework/IService.h>
#include <string>
#include <string_view>
#include <fstream>
#include <iostream>
#include <chrono>
#include <format>

enum class LogLevel : uint8_t
{
	Debug,
	Info,
	Warning,
	Error
};

enum class LogOutput : uint8_t
{
	Console = 1 << 0,
	File    = 1 << 1,
	All     = Console | File
};

inline LogOutput operator|(LogOutput a, LogOutput b)
{
	return static_cast<LogOutput>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

inline bool operator&(LogOutput a, LogOutput b)
{
	return (static_cast<uint8_t>(a) & static_cast<uint8_t>(b)) != 0;
}

class LoggingService final : public IService
{
public:
	LoggingService();
	explicit LoggingService(const std::string& logFilePath);
	~LoggingService();

	// Disable copy
	LoggingService(const LoggingService&) = delete;
	LoggingService& operator=(const LoggingService&) = delete;

	// Core logging methods
	void Log(LogLevel level, std::string_view message);
	void Log(LogLevel level, std::string_view tag, std::string_view message);

	// Convenience methods (untagged)
	void Debug(std::string_view message);
	void Info(std::string_view message);
	void Warning(std::string_view message);
	void Error(std::string_view message);

	// Configuration
	void SetMinLevel(LogLevel level);
	void SetOutput(LogOutput output);
	void SetIncludeTimestamp(bool include);

	// File management
	bool OpenLogFile(const std::string& path);
	void CloseLogFile();
	void Flush();

private:
	std::string FormatMessage(LogLevel level, std::string_view tag,
	                          std::string_view message) const;
	std::string_view LevelToString(LogLevel level) const;
	std::string GetTimestamp() const;

	std::ofstream LogFile;
	LogLevel MinLevel = LogLevel::Debug;
	LogOutput Output = LogOutput::Console;
	bool IncludeTimestamp = true;
};

// Lightweight tagged logger handle
// Components/services that need logging should store this (or just LoggingService* directly)
class Logger
{
public:
	Logger(LoggingService& service, std::string_view tag)
		: Service(&service), Tag(tag) {}

	void Debug(std::string_view message) const { Service->Log(LogLevel::Debug, Tag, message); }
	void Info(std::string_view message) const { Service->Log(LogLevel::Info, Tag, message); }
	void Warning(std::string_view message) const { Service->Log(LogLevel::Warning, Tag, message); }
	void Error(std::string_view message) const { Service->Log(LogLevel::Error, Tag, message); }

	std::string_view GetTag() const { return Tag; }

private:
	LoggingService* Service;
	std::string_view Tag;  // Caller must ensure tag outlives Logger
};
