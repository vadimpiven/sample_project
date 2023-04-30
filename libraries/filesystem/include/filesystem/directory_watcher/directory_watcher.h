#pragma once

#include <core/helpers/object.h>

#include <memory>

#include <filesystem_export.h>

namespace filesystem {

struct FILESYSTEM_EXPORT IDirectoryWatcher : core::IObject {};

using ScopedDirectoryWatcher = std::unique_ptr<IDirectoryWatcher>;

} // namespace filesystem
