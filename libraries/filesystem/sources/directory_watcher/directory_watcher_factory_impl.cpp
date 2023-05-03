#include "directory_watcher_factory_impl.h"

#include "detail/directory_watcher_impl.h"

#include <exception>

namespace filesystem {

DirectoryWatcherFactory::DirectoryWatcherFactory(std::shared_ptr<core::ILogger> logger) noexcept
    : LoggerProvider(std::move(logger))
{}

[[nodiscard]] std::unique_ptr<IDirectoryWatcher> DirectoryWatcherFactory::CreateScopedDirectoryWatcher(
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
        return std::make_unique<DirectoryWatcherImpl>(fixedPath, filter, std::move(eventOccurredCallback));
    }
    catch (const std::exception & err)
    {
		LOG_ERR << err.what();
        return {};
    }
}

[[nodiscard]] std::shared_ptr<IDirectoryWatcherFactory> FILESYSTEM_EXPORT CreateDirectoryWatcherFactory(
    std::shared_ptr<core::ILogger> logger
) noexcept
{
    return std::make_shared<DirectoryWatcherFactory>(std::move(logger));
}

} // namespace filesystem
