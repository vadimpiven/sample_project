#pragma once

#include <core/helpers/assertions.h>
#include <core/logging/logger.h>

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

protected:
    [[nodiscard]] std::ostream & LogFatal(std::source_location location = std::source_location::current()) noexcept
    {
        return m_logger->Log(LoggingLevel::Fatal, location);
    }

    [[nodiscard]] std::ostream & LogError(std::source_location location = std::source_location::current()) noexcept
    {
        return m_logger->Log(LoggingLevel::Error, location);
    }

    [[nodiscard]] std::ostream & LogWarning(std::source_location location = std::source_location::current()) noexcept
    {
        return m_logger->Log(LoggingLevel::Warning, location);
    }

    [[nodiscard]] std::ostream & LogImportant(std::source_location location = std::source_location::current()) noexcept
    {
        return m_logger->Log(LoggingLevel::Important, location);
    }

    [[nodiscard]] std::ostream & LogInfo(std::source_location location = std::source_location::current()) noexcept
    {
        return m_logger->Log(LoggingLevel::Info, location);
    }

    [[nodiscard]] std::ostream & LogDebug(std::source_location location = std::source_location::current()) noexcept
    {
        return m_logger->Log(LoggingLevel::Debug, location);
    }

private:
    const std::shared_ptr<ILogger> m_logger;
};

} // namespace core
