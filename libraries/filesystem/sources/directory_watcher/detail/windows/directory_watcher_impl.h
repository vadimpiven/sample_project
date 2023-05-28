// SPDX-License-Identifier: MIT

#pragma once

#include <filesystem/directory_watcher/directory_watcher.h>

#include <core/objects/releasable.h>

#include <array>
#include <map>
#include <stdexcept>
#include <string>
#include <thread>

#include <Windows.h>

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

        m_event = core::Releasable(
            ::CreateEventW(nullptr, true, false, nullptr), nullptr, &::CloseHandle);
        if (!m_event)
        {
            throw std::runtime_error(
                "CreateEventW failed, system error code = " + std::to_string(::GetLastError()));
        }

        m_handle = core::Releasable(
            ::FindFirstChangeNotificationW(path.c_str(), false, filterValue),
            INVALID_HANDLE_VALUE,
            &::FindCloseChangeNotification
        );
        if (!m_handle)
        {
            throw std::runtime_error(
                "FindFirstChangeNotificationW failed, system error code = " + std::to_string(::GetLastError()));
        }

        m_thread = std::thread(&DirectoryWatcherImpl::Loop, this);
    }

    ~DirectoryWatcherImpl() noexcept final
    {
        (void)::SetEvent(*m_event);
        if (m_thread.joinable())
        {
            m_thread.join();
        }
        m_handle.Release();
        m_event.Release();
    }

private:
    void Loop() const noexcept
    {
        const auto handles = std::array{*m_event, *m_handle};
        while (true)
        {
            const auto waitResult = ::WaitForMultipleObjects(
                static_cast<DWORD>(handles.size()), handles.data(), false, INFINITE);
            switch (waitResult)
            {
                case WAIT_FAILED: continue;
                case WAIT_OBJECT_0: return;
                default: m_callback();
            }
            if (!::FindNextChangeNotification(*m_handle))
            {
                return;
            }
        }
    }

private:
    const std::function<void()> m_callback;
    core::Releasable<HANDLE, decltype(&::CloseHandle)> m_event;
    core::Releasable<HANDLE, decltype(&::FindCloseChangeNotification)> m_handle;
    std::thread m_thread;
};

} // namespace filesystem::detail
