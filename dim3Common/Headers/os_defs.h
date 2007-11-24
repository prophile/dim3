#if defined(__APPLE__)
#include "mac_os_defs.h"
#elif defined(__linux__)
#include "linux_os_defs.h"
#elif defined(WIN32)
#include "win32_os_defs.h"
#else
#error unknown platform
#endif
