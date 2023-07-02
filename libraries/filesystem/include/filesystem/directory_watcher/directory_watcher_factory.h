// SPDX-License-Identifier: Apache-2.0 OR MIT

#pragma once

#include <filesystem/directory_watcher/directory_watcher.h>

#include <core/objects/object.h>

#include <filesystem>
#include <functional>
#include <memory>

#include <filesystem_export.h>

namespace filesystem {

enum class FSEventFilter
{
    FileAppendedAndClosed = 1,
    FileRenamed,
};

struct FILESYSTEM_EXPORT IDirectoryWatcherFactory : core::IObject
{
    [[nodiscard]] virtual std::unique_ptr<IDirectoryWatcher> CreateDirectoryWatcher(
        const std::filesystem::path & directoryPath,
        FSEventFilter filter,
        std::function<void()> eventOccurredCallback
    ) noexcept = 0;
};

[[nodiscard]] std::shared_ptr<IDirectoryWatcherFactory> FILESYSTEM_EXPORT CreateDirectoryWatcherFactory() noexcept;

} // namespace filesystem
