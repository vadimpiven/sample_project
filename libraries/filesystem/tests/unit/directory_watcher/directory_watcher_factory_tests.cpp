// SPDX-License-Identifier: Apache-2.0 OR MIT

#include <filesystem/directory_watcher/directory_watcher_factory.h>

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

class DirectoryWatcherTest : public TestWithParam<int>
{
public:
    [[maybe_unused]] static void SetUpTestSuite()
    {
        std::setlocale(LC_CTYPE, "en_US.utf8"); // for u8 string below
    }

protected:
    explicit DirectoryWatcherTest()
        : m_directory(std::filesystem::temp_directory_path() / u8"🐶{5941849b-3001-4467-8a02-b8e03ebf306f}")
    {
        static unsigned serial{};
        m_directory.replace_filename(m_directory.filename().string() + std::to_string(++serial));

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

    [[nodiscard]] static std::filesystem::path MakeTmpFilename(const std::filesystem::path & filename)
    {
        return std::filesystem::path(filename).replace_extension(".tmp");
    }

    [[nodiscard]] static std::string GetTestJson()
    {
        return R"({"test":"json"})";
    }

protected:
	static void WaitFilesystemEpochChange()
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(15));
	}

    static void WaitFilesystemCacheFlush()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }

    static void WaitFilesystemJournalPageFlip()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    }

private:
    std::filesystem::path m_directory;
    unsigned m_serial{};
};

TEST_F(DirectoryWatcherTest, CreateWatcherFactory)
{
    const auto factory = CreateDirectoryWatcherFactory();
    ASSERT_NE(nullptr, factory);
}

TEST_F(DirectoryWatcherTest, CreateFileWatcher)
{
    const auto factory = CreateDirectoryWatcherFactory();
    ASSERT_NE(nullptr, factory);
    const auto watcher = factory->CreateDirectoryWatcher(
        "filename", FSEventFilter::FileAppendedAndClosed, GetCallbackWithCallExpectation(0));
    ASSERT_EQ(nullptr, watcher);
}

TEST_F(DirectoryWatcherTest, CreateDirectoryWatcher)
{
    const auto factory = CreateDirectoryWatcherFactory();
    ASSERT_NE(nullptr, factory);
    const auto watcher = factory->CreateDirectoryWatcher(
        GetDirectory(), FSEventFilter::FileAppendedAndClosed, GetCallbackWithCallExpectation(0));
    ASSERT_NE(nullptr, watcher);
}

TEST_P(DirectoryWatcherTest, SetWatcher_CreateFile_WriteData_CloseFile)
{
    const auto n = GetParam();

    const auto factory = CreateDirectoryWatcherFactory();
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

TEST_F(DirectoryWatcherTest, CreateFile_SetWatcher_WriteData_CloseFile)
{
    std::ofstream file(GetUniqueFilename());

	WaitFilesystemEpochChange();

    const auto factory = CreateDirectoryWatcherFactory();
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

    const auto factory = CreateDirectoryWatcherFactory();
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

    const auto factory = CreateDirectoryWatcherFactory();
    ASSERT_NE(nullptr, factory);
    const auto watcher = factory->CreateDirectoryWatcher(
        GetDirectory(), FSEventFilter::FileAppendedAndClosed, GetCallbackWithCallExpectation(0));
    ASSERT_NE(nullptr, watcher);

    WaitFilesystemCacheFlush();
}

TEST_F(DirectoryWatcherTest, CreateFileWatcher2)
{
    const auto factory = CreateDirectoryWatcherFactory();
    ASSERT_NE(nullptr, factory);
    const auto watcher = factory->CreateDirectoryWatcher(
        "filename", FSEventFilter::FileRenamed, GetCallbackWithCallExpectation(0));
    ASSERT_EQ(nullptr, watcher);
}

TEST_F(DirectoryWatcherTest, CreateDirectoryWatcher2)
{
    const auto factory = CreateDirectoryWatcherFactory();
    ASSERT_NE(nullptr, factory);
    const auto watcher = factory->CreateDirectoryWatcher(
        GetDirectory(), FSEventFilter::FileRenamed, GetCallbackWithCallExpectation(0));
    ASSERT_NE(nullptr, watcher);
}

TEST_P(DirectoryWatcherTest, SetWatcher_CreateFile_WriteData_CloseFile_Rename)
{
    const auto n = GetParam();

    const auto factory = CreateDirectoryWatcherFactory();
    ASSERT_NE(nullptr, factory);
    const auto watcher = factory->CreateDirectoryWatcher(
        GetDirectory(), FSEventFilter::FileRenamed, GetCallbackWithCallExpectation(n));
    ASSERT_NE(nullptr, watcher);

    for (auto i = 0; i < n; ++i)
    {
        WaitFilesystemEpochChange();
        const auto filename = GetUniqueFilename(), tmp = MakeTmpFilename(filename);
        std::ofstream(tmp) << GetTestJson();
        std::filesystem::rename(tmp, filename);
    }

    WaitFilesystemJournalPageFlip(); // very long wait for macOS, or else next test receives events from current
}

TEST_F(DirectoryWatcherTest, CreateFile_SetWatcher_WriteData_CloseFile_Rename)
{
    const auto filename = GetUniqueFilename(), tmp = MakeTmpFilename(filename);
    std::ofstream file(tmp);

    WaitFilesystemEpochChange();

    const auto factory = CreateDirectoryWatcherFactory();
    ASSERT_NE(nullptr, factory);
    const auto watcher = factory->CreateDirectoryWatcher(
        GetDirectory(), FSEventFilter::FileRenamed, GetCallbackWithCallExpectation(1));
    ASSERT_NE(nullptr, watcher);

    file << GetTestJson();
    file.close();

    WaitFilesystemEpochChange();

    std::filesystem::rename(tmp, filename);

    WaitFilesystemCacheFlush();
}

TEST_F(DirectoryWatcherTest, CreateFile_WriteData_SetWatcher_CloseFile_Rename)
{
    const auto filename = GetUniqueFilename(), tmp = MakeTmpFilename(filename);
    std::ofstream file(tmp);
    file << GetTestJson();

    WaitFilesystemEpochChange();

    const auto factory = CreateDirectoryWatcherFactory();
    ASSERT_NE(nullptr, factory);
    const auto watcher = factory->CreateDirectoryWatcher(
        GetDirectory(), FSEventFilter::FileRenamed, GetCallbackWithCallExpectation(1));
    ASSERT_NE(nullptr, watcher);

    file.close();

    WaitFilesystemEpochChange();

    std::filesystem::rename(tmp, filename);

    WaitFilesystemCacheFlush();
}

TEST_F(DirectoryWatcherTest, CreateFile_WriteData_CloseFile_SetWatcher_Rename)
{
    const auto filename = GetUniqueFilename(), tmp = MakeTmpFilename(filename);
    std::ofstream(tmp) << GetTestJson();

    WaitFilesystemEpochChange();

    const auto factory = CreateDirectoryWatcherFactory();
    ASSERT_NE(nullptr, factory);
    const auto watcher = factory->CreateDirectoryWatcher(
        GetDirectory(), FSEventFilter::FileRenamed, GetCallbackWithCallExpectation(1));
    ASSERT_NE(nullptr, watcher);

    WaitFilesystemEpochChange();

    std::filesystem::rename(tmp, filename);

    WaitFilesystemCacheFlush();
}

TEST_F(DirectoryWatcherTest, CreateFile_WriteData_CloseFile_Rename_SetWatcher)
{
    const auto filename = GetUniqueFilename(), tmp = MakeTmpFilename(filename);
    std::ofstream(tmp) << GetTestJson();

    WaitFilesystemEpochChange();

    std::filesystem::rename(tmp, filename);

    WaitFilesystemEpochChange();

    const auto factory = CreateDirectoryWatcherFactory();
    ASSERT_NE(nullptr, factory);
    const auto watcher = factory->CreateDirectoryWatcher(
        GetDirectory(), FSEventFilter::FileRenamed, GetCallbackWithCallExpectation(0));
    ASSERT_NE(nullptr, watcher);

    WaitFilesystemCacheFlush();
}

INSTANTIATE_TEST_SUITE_P(Ladder, DirectoryWatcherTest, testing::Range(0, 10));

} // namespace
} // namespace filesystem
