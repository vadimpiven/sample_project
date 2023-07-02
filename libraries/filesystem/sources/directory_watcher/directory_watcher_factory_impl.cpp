// SPDX-License-Identifier: Apache-2.0 OR MIT

#include "directory_watcher_factory_impl.h"

#include "detail/directory_watcher_impl.h"

#include <spdlog/spdlog.h>

#include <exception>

namespace filesystem {

[[nodiscard]] std::unique_ptr<IDirectoryWatcher> DirectoryWatcherFactory::CreateDirectoryWatcher(
    const std::filesystem::path & directoryPath,
    const FSEventFilter filter,
    std::function<void()> eventOccurredCallback) noexcept
{
    try
    {
        if (!std::filesystem::is_directory(directoryPath))
        {
            throw std::logic_error(directoryPath.string() + " is not a directory");
        }
        const auto fixedPath = std::filesystem::canonical(directoryPath).make_preferred();
        auto watcher = std::make_unique<DirectoryWatcherImpl>(fixedPath, filter, std::move(eventOccurredCallback));
        SPDLOG_INFO("DirectoryWatcher for path \"{}\" creation succeeded", directoryPath.string());
        return watcher;
    }
    catch (const std::exception & err)
    {
        SPDLOG_ERROR("DirectoryWatcher for path \"{}\" creation failed, error: {}",
             directoryPath.string(), err.what());
        return {};
    }
}

[[nodiscard]] std::shared_ptr<IDirectoryWatcherFactory> FILESYSTEM_EXPORT CreateDirectoryWatcherFactory() noexcept
{
    return std::make_shared<DirectoryWatcherFactory>();
}

} // namespace filesystem
