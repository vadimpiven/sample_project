#pragma once

#include <filesystem/directory_watcher/directory_watcher.h>

#include <core/helpers/object.h>
#include <core/logging/logger.h>

#include <filesystem>
#include <functional>
#include <memory>

#include <filesystem_export.h>

namespace filesystem {

enum class FSEventFilter
{
    FileAppendedAndClosed = 1,
};

struct FILESYSTEM_EXPORT IDirectoryWatcherFactory : core::IObject
{
    [[nodiscard]] virtual std::unique_ptr<IDirectoryWatcher> CreateScopedDirectoryWatcher(
        const std::filesystem::path & directoryPath,
        FSEventFilter filter,
        std::function<void()> eventOccurredCallback
    ) noexcept = 0;
};

[[nodiscard]] std::shared_ptr<IDirectoryWatcherFactory> FILESYSTEM_EXPORT CreateDirectoryWatcherFactory(
    std::shared_ptr<core::ILogger> logger
) noexcept;

} // namespace filesystem
