#pragma once

#if LOGGING

#ifdef _MSC_VER
#define GET_FUNC() __FUNCTION__
#else // _MSC_VER
#define GET_FUNC() __PRETTY_FUNCTION__
#endif // _MSC_VER

#define LOG(msg, ...) Logger::Get().Log(Logger::Severity::Info, msg, __LINE__, GET_FUNC(), __FILE__, reinterpret_cast<void*>(0), __VA_ARGS__)
#define WARN(msg, ...) Logger::Get().Log(Logger::Severity::Warning, msg, __LINE__, GET_FUNC(), __FILE__, reinterpret_cast<void*>(0), __VA_ARGS__)
#define ERROR(msg, ...) Logger::Get().Log(Logger::Severity::Error, msg, __LINE__, GET_FUNC(), __FILE__, reinterpret_cast<void*>(0), __VA_ARGS__)

#else // LOGGING

#define LOG(msg, ...) 
#define WARN(msg, ...)
#define ERROR(msg, ...)

#endif // LOGGING

#if LOGGING
class Logger
{
public:
	enum class Severity : uint8
	{
		None = 0,
		Info,
		Warning,
		Error
	};

public:
	Logger(const Logger&) = delete;
	void operator=(const Logger&) = delete;

	static Logger& Get()
	{
		static Logger instance;
		return instance;
	}

	template <typename... Args>
	void Log(Severity Severity, const std::string_view& Msg, size_t Line, const char* Function, const char* File, const void* Object, Args&&... FwdArgs)
	{
		Log_Internal(Severity, std::vformat(Msg, std::make_format_args(std::forward<Args>(FwdArgs)...)), Line, Function, File, Object);
	}

	template <typename... Args>
	void Log(Severity Severity, const std::wstring_view& Msg, size_t Line, const char* Function, const char* File, const void* Object, Args&&... FwdArgs)
	{
		Log_Internal(Severity, std::vformat(Msg, std::make_wformat_args(std::forward<Args>(FwdArgs)...)), Line, Function, File, Object);
	}

private:
	Logger();

	void Initialise();

	void Log_Internal(Severity Severity, std::string_view FormattedString, size_t Line, const char* Function, const char* File, const void* Object);
	void Log_Internal(Severity Severity, std::wstring_view FormattedString, size_t Line, const char* Function, const char* File, const void* Object);
};
#endif // LOGGING
