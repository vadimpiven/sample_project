// SPDX-License-Identifier: Apache-2.0 OR MIT

#pragma once

namespace core {

struct NonCopiable
{
    NonCopiable(const NonCopiable & /*object*/) = delete;
    NonCopiable & operator=(const NonCopiable & /*object*/) = delete;

protected:
    NonCopiable() = default;
};

} // namespace core
