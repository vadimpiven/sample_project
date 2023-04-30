#pragma once

#include <filesystem/directory_watcher/directory_watcher_factory.h>

#include <gmock/gmock.h>

#include <filesystem_export.h>

namespace filesystem::testing {

struct FILESYSTEM_EXPORT DirectoryWatcherFactoryMock : IDirectoryWatcherFactory
{
    MOCK_METHOD(
        (ScopedDirectoryWatcher),
        CreateScopedDirectoryWatcher,
        (const std::filesystem::path &, FSEventFilter, std::function<void()>),
        (noexcept, override)
    );
};

} // namespace filesystem::testing
