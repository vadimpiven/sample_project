#pragma once

#include <core/helpers/object.h>

#include <iostream>
#include <memory>
#include <thread>
#include <sstream>
#include <string_view>

#include <core_export.h>

namespace core {

enum class LoggingLevel
{
	Fatal = 0,
	Error,
	Warning,
	Important,
	Info,
	Debug,
};

class CORE_EXPORT LogStream final : public std::ostringstream
{
public:
	LogStream(std::ostream & out, std::string_view prefix)
		: m_out(out.rdbuf())
	{
		(*this) << prefix;
	}

	~LogStream() noexcept final
	{
		m_out << str() << std::endl;
	}

private:
	std::ostream m_out;
};

struct CORE_EXPORT ILogger : IObject
{
    [[nodiscard]] virtual LogStream Log(
		LoggingLevel level,
		std::thread::id thread,
		std::string_view file,
		int line,
		std::string_view function
	) noexcept = 0;
};

struct CORE_EXPORT ILoggerConfigurator : ILogger
{
	virtual void SetLoggingOutput(std::unique_ptr<std::ostream> output) = 0;
	virtual void SetLoggingLevel(LoggingLevel level) noexcept = 0;
};

[[nodiscard]] std::shared_ptr<ILoggerConfigurator> CORE_EXPORT CreateLogger(
	std::unique_ptr<std::ostream> output = std::make_unique<std::ostream>(std::clog.rdbuf()),
	LoggingLevel level = LoggingLevel::Debug
) noexcept;

} // namespace core
