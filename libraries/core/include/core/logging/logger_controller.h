#pragma once

#include <core/helpers/object.h>

#include <iostream>
#include <memory>

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

struct CORE_EXPORT ILoggerSettingsSetter : IObject
{
    virtual void SetLoggingOutput(std::ostream && output) = 0;
    virtual void SetLoggingLevel(LoggingLevel level) noexcept = 0;
};

struct CORE_EXPORT ILoggerSettingsProvider : IObject
{
    [[nodiscard]] virtual std::ostream & GetLoggingOutput() noexcept = 0;
    [[nodiscard]] virtual LoggingLevel GetLoggingLevel() const noexcept = 0;
};

struct CORE_EXPORT ILoggerController : ILoggerSettingsSetter, ILoggerSettingsProvider {};

[[nodiscard]] std::shared_ptr<ILoggerController> CORE_EXPORT CreateLoggerController(
        std::ostream && output = std::ostream(std::clog.rdbuf()),
        LoggingLevel level = LoggingLevel::Debug
) noexcept;

} // namespace core
