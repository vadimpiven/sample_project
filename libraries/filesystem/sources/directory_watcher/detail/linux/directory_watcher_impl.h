#pragma once

#include <filesystem/directory_watcher/directory_watcher.h>

#include <cerrno>
#include <map>
#include <stdexcept>
#include <string>
#include <thread>

#include <linux/limits.h>
#include <sys/inotify.h>

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
        const auto filterValue = std::map<FSEventFilter, uint32_t>{
			{FSEventFilter::FileAppendedAndClosed, IN_CLOSE_WRITE | IN_DONT_FOLLOW | IN_EXCL_UNLINK | IN_ONLYDIR},
        }.at(filter);
        const auto path = absoluteDirectoryPath.string();

        m_handle = ::inotify_init();
        if (m_handle == -1)
        {
            throw std::runtime_error("inotify_init failed, errno = " + std::to_string(errno));
        }

        m_watch = ::inotify_add_watch(m_handle, path.c_str(), filterValue);
        if (m_watch == -1)
        {
            (void)::close(m_handle);
            throw std::runtime_error("inotify_add_watch failed, errno = " + std::to_string(errno));
        }

        std::thread(&DirectoryWatcherImpl::Loop, this).swap(m_thread);
    }

    ~DirectoryWatcherImpl() noexcept final
    {
        (void)::inotify_rm_watch(m_handle, m_watch);
        if (m_thread.joinable())
        {
            m_thread.join();
        }
        (void)::close(m_handle);
    }

private:
    void Loop() const noexcept
    {
        std::array<uint8_t, sizeof(struct inotify_event) + NAME_MAX + 1> buffer{};
        while (true)
        {
            auto bytesRead = ::read(m_handle, buffer.data(), buffer.size());
            if (bytesRead == -1)
            {
                return;
            }

            for (size_t i = 0; i < bytesRead; m_callback())
            {
                const auto event = reinterpret_cast<struct inotify_event *>(&buffer[i]);
                i += sizeof(struct inotify_event) + event->len;
                if (event->mask & IN_IGNORED)
                {
                    return;
                }
            }
        }
    }

private:
    const std::function<void()> m_callback;
    int m_handle;
    int m_watch;
    std::thread m_thread;
};

} // namespace filesystem::detail
