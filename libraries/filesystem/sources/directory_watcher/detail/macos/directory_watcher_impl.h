#pragma once

#include <filesystem/directory_watcher/directory_watcher.h>

#include <core/objects/releasable.h>

#include <array>
#include <map>
#include <mutex>
#include <stdexcept>
#include <utility>

#include <CoreServices/CoreServices.h>

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
		m_flags = std::map<FSEventFilter, std::pair<FSEventStreamEventFlags, FSEventStreamEventFlags>>{
			{FSEventFilter::FileAppendedAndClosed,
		 		{kFSEventStreamEventFlagItemIsFile | kFSEventStreamEventFlagItemModified, kFSEventStreamEventFlagItemRemoved}},
		}.at(filter);

        const auto path = absoluteDirectoryPath.string();
        const auto cfPath = ::CFStringCreateWithCString(nullptr, path.c_str(), kCFStringEncodingUTF8);
        if (cfPath == nullptr)
        {
            throw std::runtime_error("CFStringCreateWithCString failed");
        }

		auto paths = std::array{cfPath};
        const auto cfPaths = ::CFArrayCreate(
            nullptr, reinterpret_cast<const void **>(paths.data()), paths.size(), &kCFTypeArrayCallBacks);
        if (cfPaths == nullptr)
        {
            throw std::runtime_error("CFArrayCreate failed");
        }

		auto context = ::FSEventStreamContext{.info = this};
		m_handle = core::Releasable<FSEventStreamRef>(
            ::FSEventStreamCreate(
                nullptr,
                &DirectoryWatcherImpl::Callback,
                &context,
                cfPaths,
                kFSEventStreamEventIdSinceNow,
                CFAbsoluteTime(0),
                kFSEventStreamCreateFlagNoDefer | kFSEventStreamCreateFlagFileEvents
            ), [](FSEventStreamRef eventStream) {
                ::FSEventStreamSetDispatchQueue(eventStream, nullptr);
                ::FSEventStreamRelease(eventStream);
            });

		m_thread = core::Releasable<dispatch_queue_t, decltype(&::dispatch_release)>(
            ::dispatch_queue_create(nullptr, DISPATCH_QUEUE_CONCURRENT), &::dispatch_release);

		::FSEventStreamSetDispatchQueue(*m_handle, *m_thread);
		if (!::FSEventStreamStart(*m_handle))
		{
			throw std::runtime_error("FSEventStreamStart failed");
		}
    }

    ~DirectoryWatcherImpl() noexcept final
    {
        m_handle.Release();
		std::scoped_lock lock{m_guard};
        m_thread.Release();
    }

private:
	static void Callback(
		const ConstFSEventStreamRef /*streamRef*/,
		void * const clientCallBackInfo,
		const size_t numEvents,
		void * /*eventPaths*/,
		const FSEventStreamEventFlags eventFlags[],
		const FSEventStreamEventId /*eventIds*/[]
	)
	{
		auto self = reinterpret_cast<DirectoryWatcherImpl * const>(clientCallBackInfo);
		std::scoped_lock lock{self->m_guard};
		const auto [flagsSet, flagsUnset] = self->m_flags;
		for (size_t i = 0; i < numEvents; ++i)
		{
			if ((eventFlags[i] & flagsSet) == flagsSet && (eventFlags[i] & flagsUnset) == 0)
			{
				self->m_callback();
			}
		}
	}

private:
    const std::function<void()> m_callback;
	std::pair<FSEventStreamEventFlags, FSEventStreamEventFlags> m_flags;
    core::Releasable<dispatch_queue_t, decltype(&::dispatch_release)> m_thread;
    std::mutex m_guard;
    core::Releasable<FSEventStreamRef> m_handle;
};

} // namespace filesystem::detail
