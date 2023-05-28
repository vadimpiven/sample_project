// SPDX-License-Identifier: MIT

#pragma once

#if defined(P_LINUX)
#  include "linux/directory_watcher_impl.h"
#elif defined(P_MACOS)
#  include "macos/directory_watcher_impl.h"
#elif defined(P_WINDOWS)
#  include "windows/directory_watcher_impl.h"
#else
#  error "DirectoryWatcherEngine not implementation for your target platform"
#endif

namespace filesystem {

using DirectoryWatcherImpl = detail::DirectoryWatcherImpl;

} // namespace filesystem
