#pragma once

#include <filesystem/directory_watcher/directory_watcher.h>

#include <filesystem_export.h>

namespace filesystem::detail {

class FILESYSTEM_EXPORT DirectoryWatcherImpl final : public IDirectoryWatcher
{
public:
    DirectoryWatcherImpl(
        const std::filesystem::path & absoluteDirectoryPath,
        const FSEventFilter filter,
        std::function<void()> eventOccurredCallback
    )
        : m_callback(std::move(eventOccurredCallback))
    {
        /// https://developer.apple.com/documentation/coreservices/1443980-fseventstreamcreate?language=objc
        /// https://gist.github.com/yangacer/6037073
    }

    ~DirectoryWatcherImpl() noexcept final
    {

    }

private:
    const std::function<void()> m_callback;
};

} // namespace filesystem::detail
