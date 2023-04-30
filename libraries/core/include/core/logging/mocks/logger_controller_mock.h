#pragma once

#include <core/logging/logger_controller.h>

#include <gmock/gmock.h>

#include <core_export.h>

namespace core::testing {

struct CORE_EXPORT LoggerControllerMock : ILoggerController
{
    MOCK_METHOD((void), SetLoggingOutput, (std::ostream &&), (override));
    MOCK_METHOD((void), SetLoggingLevel, (LoggingLevel), (noexcept, override));

    MOCK_METHOD((std::ostream &), GetLoggingOutput, (), (noexcept, override));
    MOCK_METHOD((LoggingLevel), GetLoggingLevel, (), (const, noexcept, override));
};

} // namespace core::testing
