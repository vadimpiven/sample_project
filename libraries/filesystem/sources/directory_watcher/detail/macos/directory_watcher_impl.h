#pragma once

#include <filesystem/directory_watcher/directory_watcher.h>

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
		auto paths = std::array{
			::CFStringCreateWithCString(nullptr, path.c_str(), kCFStringEncodingUTF8)
		};

		auto context = ::FSEventStreamContext{.info = this};
		m_handle = ::FSEventStreamCreate(
				nullptr,
				&DirectoryWatcherImpl::Callback,
				&context,
				::CFArrayCreate(nullptr, reinterpret_cast<const void **>(paths.data()), paths.size(), nullptr),
				kFSEventStreamEventIdSinceNow,
				CFAbsoluteTime(0),
				kFSEventStreamCreateFlagNoDefer | kFSEventStreamCreateFlagFileEvents);

		m_thread = ::dispatch_queue_create(nullptr, DISPATCH_QUEUE_CONCURRENT);
		::FSEventStreamSetDispatchQueue(m_handle, m_thread);
		if (!::FSEventStreamStart(m_handle))
		{
			throw std::runtime_error("FSEventStreamStart failed");
		}
    }

    ~DirectoryWatcherImpl() noexcept final
    {
		::FSEventStreamInvalidate(m_handle);
		std::scoped_lock lock{m_guard};
		::dispatch_release(m_thread);
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
	FSEventStreamRef m_handle;
	dispatch_queue_t m_thread;
	std::mutex m_guard;
};

} // namespace filesystem::detail
