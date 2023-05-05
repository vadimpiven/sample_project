#pragma once

#include <core/error_handling/assertions.h>
#include <core/logging/logger.h>

#include <sstream>

#include <core_export.h>

namespace core {

class CORE_EXPORT LoggerProvider
{
protected:
    explicit LoggerProvider(std::shared_ptr<ILogger> logger)
        : m_logger(std::move(logger))
    {
        REQUIRE(m_logger);
    }

protected:
    [[nodiscard]] const std::shared_ptr<ILogger> & GetLogger() const noexcept
    {
        return m_logger;
    }

private:
    const std::shared_ptr<ILogger> m_logger;
};

} // namespace core

enum class LoggingLevel
{
	Fatal = 0,
	Error,
	Warning,
	Important,
	Info,
	Debug,
};

#define LOG(level) GetLogger()->Log((level), std::this_thread::get_id(), __FILE__, __LINE__, __func__)

#define LOG_FTL LOG(::core::LoggingLevel::Fatal)
#define LOG_ERR LOG(::core::LoggingLevel::Error)
#define LOG_WRN LOG(::core::LoggingLevel::Warning)
#define LOG_IMP LOG(::core::LoggingLevel::Important)
#define LOG_INF LOG(::core::LoggingLevel::Info)
#define LOG_DBG LOG(::core::LoggingLevel::Debug)
