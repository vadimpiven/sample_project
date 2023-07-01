// SPDX-License-Identifier: Apache-2.0 OR MIT

#pragma once

#include <core/objects/object.h>
#include <core/objects/non_copiable.h>

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

class CORE_EXPORT LogStream final : public std::ostringstream, public NonCopiable
{
public:
	LogStream(std::ostream & ostream, std::string_view prefix)
		: m_ostream(ostream.rdbuf())
	{
		(*this) << prefix;
	}

	~LogStream() noexcept final
	{
        m_ostream << str() << std::endl;
	}

private:
	std::ostream m_ostream;
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
