#pragma once

#include <core/logging/logger.h>

#include <ostream>

#include <core_export.h>

namespace core::testing {

class CORE_EXPORT LoggerDouble final : public ILogger
{
public:
    [[nodiscard]] std::ostream & Log(LoggingLevel /*level*/, std::source_location /*location*/) noexcept final
    {
        return m_null;
    }

private:
    std::ostream m_null{nullptr};
};

} // namespace core::testing
