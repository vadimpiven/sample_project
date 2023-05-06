#pragma once

#include <filesystem/directory_watcher/directory_watcher_factory.h>

#include <core/logging/logger_provider.h>

#include <filesystem_export.h>

namespace filesystem {

class FILESYSTEM_EXPORT DirectoryWatcherFactory final
    : public IDirectoryWatcherFactory
    , protected core::LoggerProvider
{
public:
    explicit DirectoryWatcherFactory(std::shared_ptr<core::ILogger> logger) noexcept;

public:
    [[nodiscard]] std::unique_ptr<IDirectoryWatcher> CreateDirectoryWatcher(
        const std::filesystem::path & directoryPath,
        FSEventFilter filter,
        std::function<void()> eventOccurredCallback
    ) noexcept final;
};

} // namespace filesystem
