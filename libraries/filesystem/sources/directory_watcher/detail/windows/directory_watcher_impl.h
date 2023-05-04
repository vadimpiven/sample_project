#pragma once

#include <filesystem/directory_watcher/directory_watcher.h>

#include <Windows.h>

#include <array>
#include <map>
#include <stdexcept>
#include <string>
#include <thread>

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
            {FSEventFilter::FileAppendedAndClosed, FILE_NOTIFY_CHANGE_LAST_WRITE},
        }.at(filter);
        const auto path = LR"(\\?\)" + absoluteDirectoryPath.wstring() + LR"(\)";

        m_event = ::CreateEventW(nullptr, true, false, nullptr);
        if (m_event == nullptr)
        {
            throw std::runtime_error(
                "CreateEventW failed, system error code = " + std::to_string(::GetLastError()));
        }

        m_handle = ::FindFirstChangeNotificationW(path.c_str(), false, filterValue);
        if (m_handle == INVALID_HANDLE_VALUE)
        {
            (void)::CloseHandle(m_event);
            throw std::runtime_error(
                "FindFirstChangeNotificationW failed, system error code = " + std::to_string(::GetLastError()));
        }

        std::thread(&DirectoryWatcherImpl::Loop, this).swap(m_thread);
    }

    ~DirectoryWatcherImpl() noexcept final
    {
        (void)::SetEvent(m_event);
        if (m_thread.joinable())
        {
            m_thread.join();
        }
        (void)::FindCloseChangeNotification(m_handle);
        (void)::CloseHandle(m_event);
    }

private:
    void Loop() const noexcept
    {
        const auto handles = std::array{m_event, m_handle};
        while (true)
        {
            const auto waitResult =
                ::WaitForMultipleObjects(handles.size(), handles.data(), false, INFINITE);
            switch (waitResult)
            {
                case WAIT_FAILED: continue;
                case WAIT_OBJECT_0: return;
                default: m_callback();
            }
            if (!::FindNextChangeNotification(m_handle))
            {
                return;
            }
        }
    }

private:
    const std::function<void()> m_callback;
    HANDLE m_handle;
    HANDLE m_event;
    std::thread m_thread;
};

} // namespace filesystem::detail
