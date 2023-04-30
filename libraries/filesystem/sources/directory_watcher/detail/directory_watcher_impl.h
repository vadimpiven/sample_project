#pragma once

#if defined(__linux__)
#  include "linux/directory_watcher_impl.h"
#elif defined(__APPLE__)
#  include "macos/directory_watcher_impl.h"
#elif defined(_WIN32)
#  include "windows/directory_watcher_impl.h"
#else
#  error "DirectoryWatcherEngine not implementation for your target platform"
#endif

namespace filesystem {

using DirectoryWatcherImpl = detail::DirectoryWatcherImpl;

} // namespace filesystem
