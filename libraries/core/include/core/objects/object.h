// SPDX-License-Identifier: Apache-2.0 OR MIT

#pragma once

#include <core_export.h>

namespace core {

struct CORE_EXPORT IObject
{
    virtual ~IObject() noexcept = default;
};

} // namespace core
