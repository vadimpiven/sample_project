﻿#include <filesystem/directory_watcher/directory_watcher_factory.h>

#include <core/logging/doubles/logger_double.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <clocale>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <thread>

namespace filesystem::testing {
namespace {

using namespace ::testing;
using namespace core::testing;

class DirectoryWatcherTest : public TestWithParam<int>
{
public:
    [[maybe_unused]] static void SetUpTestSuite()
    {
        std::setlocale(LC_CTYPE, "en_US.utf8"); // for u8 string below
    }

protected:
    explicit DirectoryWatcherTest()
        : m_directory(std::filesystem::current_path() / u8"🐶{5941849b-3001-4467-8a02-b8e03ebf306f}")
        , m_logger(std::make_shared<LoggerDouble>())
    {
        std::filesystem::remove_all(m_directory);
        std::filesystem::create_directory(m_directory);
        std::filesystem::permissions(m_directory, std::filesystem::perms::owner_all);
    }

    ~DirectoryWatcherTest() noexcept override
    {
        std::error_code concealError;
        std::filesystem::remove_all(m_directory, concealError);
    }

    void SetUp() override
    {
#ifdef SKIP_TEDIOUS_TESTS
        GTEST_SKIP() << "DirectoryWatcherTest skipped, remove SKIP_TEDIOUS_TESTS define to enable them";
#else
        std::cout << "warning: DirectoryWatcherTest test suit is tedious, "
                     "you could disable it by defining SKIP_TEDIOUS_TESTS" << std::endl;
#endif
    }

protected:
    [[nodiscard]] std::shared_ptr<core::ILogger> GetLogger() const
    {
        return m_logger;
    }

protected:
    [[nodiscard]] const std::filesystem::path & GetDirectory() const
    {
        return m_directory;
    }

private:
    MOCK_METHOD(void, TestCallback, ());

protected:
    [[nodiscard]] std::function<void()> GetCallbackWithCallExpectation(const int n)
    {
        EXPECT_CALL(*this, TestCallback()).Times(n);
        return [this]{ TestCallback(); };
    }

protected:
    [[nodiscard]] std::filesystem::path GetUniqueFilename()
    {
        auto name = (std::stringstream{} << std::hex << std::setfill('0') << std::setw(8) << ++m_serial).str() + ".json";
        return m_directory / std::move(name);
    }

    [[nodiscard]] static std::string GetTestJson()
    {
        return R"({"test":"json"})";
    }

protected:
	static void WaitFilesystemEpochChange()
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

    static void WaitFilesystemCacheFlush()
    {
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }

private:
    const std::filesystem::path m_directory;
    unsigned m_serial{};

private:
    const std::shared_ptr<LoggerDouble> m_logger;
};

TEST_F(DirectoryWatcherTest, CreateWatcherFactory)
{
    const auto factory = CreateDirectoryWatcherFactory(GetLogger());
    ASSERT_NE(nullptr, factory);
}

TEST_F(DirectoryWatcherTest, CreateFileWatcher)
{
    const auto factory = CreateDirectoryWatcherFactory(GetLogger());
    ASSERT_NE(nullptr, factory);
    const auto watcher = factory->CreateDirectoryWatcher(
        "filename", FSEventFilter::FileAppendedAndClosed, GetCallbackWithCallExpectation(0));
    ASSERT_EQ(nullptr, watcher);
}

TEST_F(DirectoryWatcherTest, CreateDirectoryWatcher)
{
    const auto factory = CreateDirectoryWatcherFactory(GetLogger());
    ASSERT_NE(nullptr, factory);
    const auto watcher = factory->CreateDirectoryWatcher(
        GetDirectory(), FSEventFilter::FileAppendedAndClosed, GetCallbackWithCallExpectation(0));
    ASSERT_NE(nullptr, watcher);
}

TEST_P(DirectoryWatcherTest, SetWatcher_CreateFile_WriteData_CloseFile)
{
    const auto n = GetParam();

    const auto factory = CreateDirectoryWatcherFactory(GetLogger());
    ASSERT_NE(nullptr, factory);
    const auto watcher = factory->CreateDirectoryWatcher(
        GetDirectory(), FSEventFilter::FileAppendedAndClosed, GetCallbackWithCallExpectation(n));
    ASSERT_NE(nullptr, watcher);

    for (auto i = 0; i < n; ++i)
    {
		WaitFilesystemEpochChange();
        std::ofstream(GetUniqueFilename()) << GetTestJson();
    }

    WaitFilesystemCacheFlush();
}

INSTANTIATE_TEST_SUITE_P(Ladder, DirectoryWatcherTest, testing::Range(0, 10));

TEST_F(DirectoryWatcherTest, CreateFile_SetWatcher_WriteData_CloseFile)
{
    std::ofstream file(GetUniqueFilename());

	WaitFilesystemEpochChange();

    const auto factory = CreateDirectoryWatcherFactory(GetLogger());
    ASSERT_NE(nullptr, factory);
    const auto watcher = factory->CreateDirectoryWatcher(
        GetDirectory(), FSEventFilter::FileAppendedAndClosed, GetCallbackWithCallExpectation(1));
    ASSERT_NE(nullptr, watcher);

    file << GetTestJson();
    file.close();

    WaitFilesystemCacheFlush();
}

TEST_F(DirectoryWatcherTest, CreateFile_WriteData_SetWatcher_CloseFile)
{
    std::ofstream file(GetUniqueFilename());
    file << GetTestJson();

	WaitFilesystemEpochChange();

    const auto factory = CreateDirectoryWatcherFactory(GetLogger());
    ASSERT_NE(nullptr, factory);
    const auto watcher = factory->CreateDirectoryWatcher(
        GetDirectory(), FSEventFilter::FileAppendedAndClosed, GetCallbackWithCallExpectation(1));
    ASSERT_NE(nullptr, watcher);

    file.close();

    WaitFilesystemCacheFlush();
}

TEST_F(DirectoryWatcherTest, CreateFile_WriteData_CloseFile_SetWatcher)
{
    {
        std::ofstream(GetUniqueFilename()) << GetTestJson();
    }

	WaitFilesystemEpochChange();

    const auto factory = CreateDirectoryWatcherFactory(GetLogger());
    ASSERT_NE(nullptr, factory);
    const auto watcher = factory->CreateDirectoryWatcher(
        GetDirectory(), FSEventFilter::FileAppendedAndClosed, GetCallbackWithCallExpectation(0));
    ASSERT_NE(nullptr, watcher);

    WaitFilesystemCacheFlush();
}

} // namespace
} // namespace filesystem