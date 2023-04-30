#pragma once

#include <core/logging/logger.h>

#include <gmock/gmock.h>

#include <core_export.h>

namespace core::testing {

struct CORE_EXPORT LoggerMock : ILogger
{
    MOCK_METHOD((std::ostream &), Log, (LoggingLevel, std::source_location), (noexcept, override));
};

} // namespace core::testing
