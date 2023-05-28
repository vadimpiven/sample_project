// SPDX-License-Identifier: MIT

#pragma once

#include <stdexcept>

#define EXPECT(x) if (!(x)) throw std::logic_error(#x "expectation not satisfied")

#define REQUIRE(x) EXPECT(x)

#define ENSURE(x) EXPECT(x)
