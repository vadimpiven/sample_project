// SPDX-License-Identifier: MIT

#include <filesystem/directory_watcher/directory_watcher_factory.h>

#include <core/error_handling/assertions.h>
#include <core/logging/doubles/logger_double.h>

#include <exception>
#include <filesystem>
#include <iostream>
#include <memory>

int main()
{
    try
    {
        const auto directory = std::filesystem::current_path() / "{d25216c5-9312-45cb-82db-9650d20098bb}";
        const auto logger(std::make_shared<core::testing::LoggerDouble>());

        std::filesystem::create_directory(directory);

        const auto factory = filesystem::CreateDirectoryWatcherFactory(logger);
        ENSURE(factory);
        const auto watcher = factory->CreateDirectoryWatcher(
            directory, filesystem::FSEventFilter::FileAppendedAndClosed, []{});
        ENSURE(watcher);

        std::filesystem::remove_all(directory);
    }
    catch (const std::exception & err)
    {
        std::cerr << err.what() << std::endl;
    }
    return 0;
}
