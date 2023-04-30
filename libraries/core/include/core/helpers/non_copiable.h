#pragma once

#include <core/helpers/object.h>

#include <core_export.h>

namespace core {

struct CORE_EXPORT NonCopiable : IObject
{
    NonCopiable(const NonCopiable &) = delete;
    NonCopiable & operator=(const NonCopiable &) = delete;

protected:
    NonCopiable() = default;
};

} // namespace core
