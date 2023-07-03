// Copyright (c) 2023 Sebastian Ploch
#include "Log.h"

#if LOGGING

#define PLOG_OMIT_LOG_DEFINES

#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>
#include <plog/Appenders/DebugOutputAppender.h>
#include <filesystem>

[[nodiscard]] static plog::Severity GetPLogSeverity(Logger::Severity Severity)
{
	switch (Severity)
	{
	case Logger::Severity::None:
		return plog::Severity::none;
	case Logger::Severity::Info:
		return plog::Severity::info;
	case Logger::Severity::Warning:
		return plog::Severity::warning;
	case Logger::Severity::Error:
		return plog::Severity::error;
	default:
		throw std::logic_error("unhandled case");
	}
}

Logger::Logger()
{
	Initialise();
}

void Logger::Initialise()
{
	// Remove previous log
	const std::filesystem::path logFile = "Log.txt";
	if (std::filesystem::exists(logFile))
	{
		std::filesystem::remove(logFile);
	}

	// Init plog
	plog::Logger<0>& log = plog::init(plog::verbose, "Log.txt");

	static plog::DebugOutputAppender<plog::TxtFormatter> debugOutputAppender {};
	log.addAppender(&debugOutputAppender);
}

void Logger::Log_Internal(Severity Severity, std::string_view FormattedString, size_t Line, const char* Function, const char* File, const void* Object)
{
	const plog::Severity plogSeverity = GetPLogSeverity(Severity);
	IF_PLOG(plogSeverity)
	{
		*plog::get<0>() += plog::Record(plogSeverity, Function, Line, File, Object, 0).ref() << FormattedString;
	}
}

void Logger::Log_Internal(Severity Severity, std::wstring_view FormattedString, size_t Line, const char* Function, const char* File, const void* Object)
{
	const plog::Severity plogSeverity = GetPLogSeverity(Severity);
	IF_PLOG(plogSeverity)
	{
		*plog::get<0>() += plog::Record(plogSeverity, Function, Line, File, Object, 0).ref() << FormattedString;
	}
}
#endif // LOGGING
