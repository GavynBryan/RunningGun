#include <core/services/logging/LoggingService.h>

LoggingService::LoggingService()
{
}

LoggingService::LoggingService(const std::string& logFilePath)
{
	OpenLogFile(logFilePath);
}

LoggingService::~LoggingService()
{
	CloseLogFile();
}

void LoggingService::Log(LogLevel level, std::string_view message)
{
	Log(level, "", message);
}

void LoggingService::Log(LogLevel level, std::string_view tag, std::string_view message)
{
	if (level < MinLevel) {
		return;
	}
	
	std::string formatted = FormatMessage(level, tag, message);
	
	if (Output & LogOutput::Console) {
		std::cout << formatted << std::endl;
	}
	
	if ((Output & LogOutput::File) && LogFile.is_open()) {
		LogFile << formatted << std::endl;
	}
}

void LoggingService::Debug(std::string_view message)
{
	Log(LogLevel::Debug, message);
}

void LoggingService::Info(std::string_view message)
{
	Log(LogLevel::Info, message);
}

void LoggingService::Warning(std::string_view message)
{
	Log(LogLevel::Warning, message);
}

void LoggingService::Error(std::string_view message)
{
	Log(LogLevel::Error, message);
}

void LoggingService::SetMinLevel(LogLevel level)
{
	MinLevel = level;
}

void LoggingService::SetOutput(LogOutput output)
{
	Output = output;
}

void LoggingService::SetIncludeTimestamp(bool include)
{
	IncludeTimestamp = include;
}

bool LoggingService::OpenLogFile(const std::string& path)
{
	CloseLogFile();
	LogFile.open(path, std::ios::out | std::ios::app);
	return LogFile.is_open();
}

void LoggingService::CloseLogFile()
{
	if (LogFile.is_open()) {
		LogFile.flush();
		LogFile.close();
	}
}

void LoggingService::Flush()
{
	if (LogFile.is_open()) {
		LogFile.flush();
	}
	std::cout.flush();
}

std::string LoggingService::FormatMessage(LogLevel level, std::string_view tag, 
                                          std::string_view message) const
{
	std::string result;
	
	if (IncludeTimestamp) {
		result += "[" + GetTimestamp() + "] ";
	}
	
	result += "[";
	result += LevelToString(level);
	result += "]";
	
	if (!tag.empty()) {
		result += " [";
		result += tag;
		result += "]";
	}
	
	result += " ";
	result += message;
	
	return result;
}

std::string_view LoggingService::LevelToString(LogLevel level) const
{
	switch (level) {
		case LogLevel::Debug:   return "DEBUG";
		case LogLevel::Info:    return "INFO";
		case LogLevel::Warning: return "WARN";
		case LogLevel::Error:   return "ERROR";
		default:                return "UNKNOWN";
	}
}

std::string LoggingService::GetTimestamp() const
{
	auto now = std::chrono::system_clock::now();
	auto time = std::chrono::system_clock::to_time_t(now);
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
		now.time_since_epoch()) % 1000;
	
	std::tm tm_buf;
#ifdef _WIN32
	localtime_s(&tm_buf, &time);
#else
	localtime_r(&time, &tm_buf);
#endif
	
	return std::format("{:02}:{:02}:{:02}.{:03}",
		tm_buf.tm_hour, tm_buf.tm_min, tm_buf.tm_sec, ms.count());
}
