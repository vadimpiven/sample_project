// SPDX-License-Identifier: Apache-2.0 OR MIT

#pragma once

#include <filesystem/directory_watcher/directory_watcher_factory.h>

#include <gmock/gmock.h>

#include <filesystem_export.h>

namespace filesystem::testing {

struct FILESYSTEM_EXPORT DirectoryWatcherFactoryMock : IDirectoryWatcherFactory
{
    MOCK_METHOD(
        (std::unique_ptr<IDirectoryWatcher>),
        CreateDirectoryWatcher,
        (const std::filesystem::path &, FSEventFilter, std::function<void()>),
        (noexcept, override)
    );
};

} // namespace filesystem::testing
