#pragma once

#include <filesystem/directory_watcher/directory_watcher.h>

#include <core/helpers/assertions.h>

#include <Windows.h>

#include <array>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <thread>

#include <filesystem_export.h>

namespace filesystem::detail {

[[nodiscard]] std::wstring Utf8ToUtf16(const std::string & string)
{
    if (string.empty())
    {
        return {};
    }

    const auto expectedLength = ::MultiByteToWideChar(
        CP_UTF8, 0,
        string.data(), static_cast<int>(string.size()),
        nullptr, 0
    );
    if (expectedLength == 0)
    {
        throw std::runtime_error(
            "MultiByteToWideChar failed calculating size, error = " + std::to_string(GetLastError()));
    }

    auto result = std::wstring(static_cast<std::wstring::size_type>(expectedLength), '\0');

    const auto actualLength = ::MultiByteToWideChar(
            CP_UTF8, 0,
            string.data(), static_cast<int>(string.size()),
            result.data(), static_cast<int>(result.size())
    );
    if (actualLength != expectedLength)
    {
        throw std::runtime_error(
            "MultiByteToWideChar failed converting string, error = " + std::to_string(GetLastError()));
    }

    return result;
}

//[[nodiscard]] std::string Utf16ToUtf8(const std::wstring & string)
//{
//    if (string.empty())
//    {
//        return {};
//    }
//
//    const auto expectedLength = ::WideCharToMultiByte(
//        CP_UTF8, 0,
//        string.data(), static_cast<int>(string.size()),
//        nullptr, 0,
//        nullptr, nullptr
//    );
//    if (expectedLength == 0)
//    {
//        throw std::runtime_error(
//            "WideCharToMultiByte failed calculating size, error = " + std::to_string(GetLastError()));
//    }
//
//    auto result = std::string(static_cast<std::wstring::size_type>(expectedLength), '\0');
//
//    const auto actualLength = ::WideCharToMultiByte(
//        CP_UTF8, 0,
//        string.data(), static_cast<int>(string.size()),
//        result.data(), static_cast<int>(result.size()),
//        nullptr, nullptr
//    );
//    if (actualLength != expectedLength)
//    {
//        throw std::runtime_error(
//            "WideCharToMultiByte failed converting string, error = " + std::to_string(GetLastError()));
//    }
//
//    return result;
//}

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
        const auto path = LR"(\\?\)" + Utf8ToUtf16(absoluteDirectoryPath.string());
        m_event = ::CreateEventW(nullptr, true, false, nullptr);
        ENSURE(m_event);
        m_handle = ::FindFirstChangeNotificationW(path.c_str(), false, filterValue);
        ENSURE(m_handle);
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
    void Loop() noexcept
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
