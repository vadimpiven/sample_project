// SPDX-License-Identifier: Apache-2.0 OR MIT

#pragma once

#include <filesystem/directory_watcher/directory_watcher_factory.h>

#include <filesystem_export.h>

namespace filesystem {

class FILESYSTEM_EXPORT DirectoryWatcherFactory final
    : public IDirectoryWatcherFactory
{
public:
    [[nodiscard]] std::unique_ptr<IDirectoryWatcher> CreateDirectoryWatcher(
        const std::filesystem::path & directoryPath,
        FSEventFilter filter,
        std::function<void()> eventOccurredCallback
    ) noexcept final;
};

} // namespace filesystem
