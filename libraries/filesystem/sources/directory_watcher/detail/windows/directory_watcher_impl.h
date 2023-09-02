// SPDX-License-Identifier: Apache-2.0 OR MIT

#pragma once

#include <filesystem/directory_watcher/directory_watcher.h>

#include <core/error_handling/suppressions.h>
#include <core/objects/releasable.h>

#include <array>
#include <map>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>

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
        m_flags = std::map<FSEventFilter, std::pair<DWORD, DWORD>>{
            {FSEventFilter::FileAppendedAndClosed, {FILE_NOTIFY_CHANGE_LAST_WRITE, 0}},
            {FSEventFilter::FileRenamed, {FILE_NOTIFY_CHANGE_FILE_NAME, FILE_ACTION_RENAMED_NEW_NAME}},
        }.at(filter);
        const auto path = LR"(\\?\)" + absoluteDirectoryPath.wstring() + LR"(\)";

        m_directory = core::Releasable(
            ::CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
                OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, nullptr),
            INVALID_HANDLE_VALUE, &::CloseHandle);
        if (!m_directory)
        {
            throw std::runtime_error(
                "CreateFileW failed, system error code = " + std::to_string(::GetLastError()));
        }

        m_notifyEvent = core::Releasable(
            ::CreateEventW(nullptr, false, false, nullptr), nullptr, &::CloseHandle);
        if (!m_notifyEvent)
        {
            throw std::runtime_error(
                "CreateEventW failed, system error code = " + std::to_string(::GetLastError()));
        }

        m_overlapped = std::make_unique<OVERLAPPED>(OVERLAPPED{.hEvent = *m_notifyEvent});

        m_stopEvent = core::Releasable(
            ::CreateEventW(nullptr, true, false, nullptr), nullptr, &::CloseHandle);
        if (!m_stopEvent)
        {
            throw std::runtime_error(
                "CreateEventW failed, system error code = " + std::to_string(::GetLastError()));
        }

        // 32,767 is max path length (https://learn.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation?tabs=registry), we will use smaller size
        m_information.resize(sizeof(FILE_NOTIFY_INFORMATION) + path.size() + MAX_PATH);

        if (!::ReadDirectoryChangesW(*m_directory, m_information.data(), static_cast<DWORD>(m_information.size()),
             true, m_flags.first, nullptr, m_overlapped.get(), nullptr))
        {
            throw std::runtime_error(
                "ReadDirectoryChangesW failed, system error code = " + std::to_string(::GetLastError()));
        }

        m_thread = std::thread(&DirectoryWatcherImpl::Loop, this);
    }

    ~DirectoryWatcherImpl() noexcept final
    {
        UNUSED(::CancelIoEx(*m_directory, m_overlapped.get()));
        UNUSED(::SetEvent(*m_stopEvent));
        if (m_thread.joinable())
        {
            m_thread.join();
        }
    }

private:
    void Loop() noexcept
    {
        const auto handles = std::array{*m_stopEvent, *m_notifyEvent};
        while (true)
        {
            const auto waitResult = ::WaitForMultipleObjects(
                static_cast<DWORD>(handles.size()), handles.data(), false, INFINITE);
            switch (waitResult)
            {
                case WAIT_FAILED: continue;
                case WAIT_OBJECT_0: return;
                default: break;
            }

            DWORD bytesTransferred{};
            if (!::GetOverlappedResult(*m_directory, m_overlapped.get(), &bytesTransferred, false))
            {
                return;
            }
            if (bytesTransferred)
            {
                Process();
            }
            if (!::ReadDirectoryChangesW(*m_directory, m_information.data(), static_cast<DWORD>(m_information.size()),
                 true, m_flags.first, nullptr, m_overlapped.get(), nullptr))
            {
                return;
            }
        }
    }

    void Process() const noexcept
    {
        DWORD offset{};
        auto data = reinterpret_cast<uintptr_t>(m_information.data());
        do
        {
            data += offset;
            auto info = reinterpret_cast<const FILE_NOTIFY_INFORMATION * const>(data);
            if ((info->Action & m_flags.second) == m_flags.second)
            {
                m_callback();
            }
            offset = info->NextEntryOffset;
        }
        while (offset != 0);
    }

private:
    const std::function<void()> m_callback;
    std::pair<DWORD, DWORD> m_flags{};
    std::unique_ptr<OVERLAPPED> m_overlapped;
    std::vector<DWORD> m_information;
    core::Releasable<HANDLE, decltype(&::CloseHandle)> m_directory, m_notifyEvent, m_stopEvent;
    std::thread m_thread;
};

} // namespace filesystem::detail
