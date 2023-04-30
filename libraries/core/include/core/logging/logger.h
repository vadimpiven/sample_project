#pragma once

#include <core/logging/logger_controller.h>

#include <source_location>

#include <core_export.h>

namespace core {

struct CORE_EXPORT ILogger : IObject
{
    [[nodiscard]] virtual std::ostream & Log(LoggingLevel level, std::source_location location) noexcept = 0;
};

[[nodiscard]] std::shared_ptr<ILogger> CORE_EXPORT CreateLogger(
    std::shared_ptr<ILoggerSettingsProvider> settingsProvider
) noexcept;

} // namespace core
