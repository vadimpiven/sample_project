// SPDX-License-Identifier: MIT

#pragma once

#include <core/logging/logger.h>

#include <ostream>

#include <core_export.h>

namespace core::testing {

class CORE_EXPORT LoggerDouble final : public ILogger
{
public:
    [[nodiscard]] LogStream Log(
		LoggingLevel /*level*/,
		std::thread::id /*thread*/,
		std::string_view /*file*/,
		int /*line*/,
		std::string_view /*function*/
	) noexcept final
    {
        return {m_null, ""};
    }

private:
    std::ostream m_null{nullptr};
};

} // namespace core::testing
