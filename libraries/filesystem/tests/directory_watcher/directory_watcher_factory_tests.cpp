#include <filesystem/directory_watcher/directory_watcher_factory.h>

#include <core/logging/doubles/logger_double.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
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
        GTEST_FLAG_SET(death_test_style, "threadsafe");
        //std::setlocale(LC_CTYPE, "en_US.utf8");
    }

protected:
    explicit DirectoryWatcherTest()
        : m_directory(std::filesystem::temp_directory_path() / u8"🐶{5941849b-3001-4467-8a02-b8e03ebf306f}")
        , m_logger(std::make_shared<LoggerDouble>())
    {
        std::filesystem::remove_all(m_directory);
        std::filesystem::create_directory(m_directory);
        std::filesystem::permissions(m_directory, std::filesystem::perms::owner_all);
    }

    ~DirectoryWatcherTest() noexcept override
    {
        std::filesystem::remove_all(m_directory);
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
        return m_directory / (std::stringstream{} << std::hex << std::setfill('0') << std::setw(8) << ++m_serial).str();
    }

    [[nodiscard]] static std::string GetTestJson()
    {
        return R"({"test":"json"})";
    }

protected:
    static void WaitFilesystemCacheFlush()
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
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
    const auto watcher = factory->CreateScopedDirectoryWatcher(
            "filename", FSEventFilter::FileContentChanged, GetCallbackWithCallExpectation(0));
    ASSERT_EQ(nullptr, watcher);
}

TEST_F(DirectoryWatcherTest, CreateDirectoryWatcher)
{
    const auto factory = CreateDirectoryWatcherFactory(GetLogger());
    ASSERT_NE(nullptr, factory);
    const auto watcher = factory->CreateScopedDirectoryWatcher(
            GetDirectory(), FSEventFilter::FileContentChanged, GetCallbackWithCallExpectation(0));
    ASSERT_NE(nullptr, watcher);
}

TEST_P(DirectoryWatcherTest, SetWatcher_CreateFile_WriteData_CloseFile)
{
    const auto n = GetParam();

    const auto factory = CreateDirectoryWatcherFactory(GetLogger());
    ASSERT_NE(nullptr, factory);
    const auto watcher = factory->CreateScopedDirectoryWatcher(
        GetDirectory(), FSEventFilter::FileContentChanged, GetCallbackWithCallExpectation(n));
    ASSERT_NE(nullptr, watcher);

    for (auto i = 0; i < n; ++i)
    {
        std::fstream{GetUniqueFilename()} << GetTestJson();
    }

    WaitFilesystemCacheFlush();
}

INSTANTIATE_TEST_SUITE_P(Ladder, DirectoryWatcherTest, testing::Range(0, 10));

TEST_F(DirectoryWatcherTest, CreateFile_SetWatcher_WriteData_CloseFile)
{
    std::fstream file{GetUniqueFilename()};

    const auto factory = CreateDirectoryWatcherFactory(GetLogger());
    ASSERT_NE(nullptr, factory);
    const auto watcher = factory->CreateScopedDirectoryWatcher(
        GetDirectory(), FSEventFilter::FileContentChanged, GetCallbackWithCallExpectation(1));
    ASSERT_NE(nullptr, watcher);

    file << GetTestJson();
    file.close();

    WaitFilesystemCacheFlush();
}

TEST_F(DirectoryWatcherTest, CreateFile_WriteData_SetWatcher_CloseFile)
{
    std::fstream file{GetUniqueFilename()};
    file << GetTestJson();

    const auto factory = CreateDirectoryWatcherFactory(GetLogger());
    ASSERT_NE(nullptr, factory);
    const auto watcher = factory->CreateScopedDirectoryWatcher(
        GetDirectory(), FSEventFilter::FileContentChanged, GetCallbackWithCallExpectation(1));
    ASSERT_NE(nullptr, watcher);

    file.close();

    WaitFilesystemCacheFlush();
}

TEST_F(DirectoryWatcherTest, CreateFile_WriteData_CloseFile_SetWatcher)
{
    {
        std::fstream{GetUniqueFilename()} << GetTestJson();
    }

    const auto factory = CreateDirectoryWatcherFactory(GetLogger());
    ASSERT_NE(nullptr, factory);
    const auto watcher = factory->CreateScopedDirectoryWatcher(
        GetDirectory(), FSEventFilter::FileContentChanged, GetCallbackWithCallExpectation(0));
    ASSERT_NE(nullptr, watcher);

    WaitFilesystemCacheFlush();
}

} // namespace
} // namespace filesystem
