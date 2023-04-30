#pragma once

#include <filesystem/directory_watcher/directory_watcher.h>

#include <Windows.h>

#include <map>
#include <string>

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
        const auto filterValue = std::map<FSEventFilter, DWORD>{
            {FSEventFilter::FileContentChanged, FILE_NOTIFY_CHANGE_LAST_WRITE},
        }.at(filter);
    }

    ~DirectoryWatcherImpl() noexcept override
    {

    }

private:
    const std::function<void()> m_callback;
};

} // namespace filesystem::detail
