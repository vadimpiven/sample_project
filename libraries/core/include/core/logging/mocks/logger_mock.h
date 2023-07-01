// SPDX-License-Identifier: Apache-2.0 OR MIT

#pragma once

#include <core/logging/logger.h>

#include <gmock/gmock.h>

#include <core_export.h>

namespace core::testing {

struct CORE_EXPORT LoggerMock : ILogger
{
    MOCK_METHOD(
		(LogStream),
		Log,
		(LoggingLevel, std::thread::id, std::string_view, int, std::string_view),
		(noexcept, override)
	);
};

struct CORE_EXPORT LoggerConfiguratorMock : ILoggerConfigurator, LoggerMock
{
	MOCK_METHOD((void), SetLoggingOutput, (std::unique_ptr<std::ostream>), (override));
	MOCK_METHOD((void), SetLoggingLevel, (LoggingLevel), (noexcept, override));
};

} // namespace core::testing
