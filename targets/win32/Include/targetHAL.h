//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef TARGET_HAL_H
#define TARGET_HAL_H

// #include <nanoHAL_Power.h>

#ifdef _WIN32
#define PLATFORM_DELAY(milliSecs) Sleep(milliSecs);
#else
#include <unistd.h>
#define PLATFORM_DELAY(milliSecs) usleep((milliSecs) * 1000);
#endif

// set min possible number of sockets
#define PLATFORM_DEPENDENT__SOCKETS_MAX_COUNT 1

#if defined(VIRTUAL_DEVICE) && defined(_WIN32)
#define NANOCLR_STOP() ::DebugBreak()
#pragma warning(error : 4706) // error C4706: assignment within conditional expression
#elif defined(VIRTUAL_DEVICE)
#include <csignal>
#define NANOCLR_STOP() raise(SIGTRAP)
#endif

#if !defined(BUILD_RTM)

inline void HARD_Breakpoint()
{
#ifdef _WIN32
    if (::IsDebuggerPresent())
    {
        ::DebugBreak();
    }
#else
    raise(SIGTRAP);
#endif
}

#define HARD_BREAKPOINT() HARD_Breakpoint()

inline bool Target_ConfigUpdateRequiresErase()
{
    return true;
}

#endif // !defined(BUILD_RTM)

inline bool Target_HasNanoBooter()
{
    return false;
};

inline bool Target_CanChangeMacAddress()
{
    return false;
};

inline bool Target_IFUCapable()
{
    return false;
};

inline bool Target_HasProprietaryBooter()
{
    return false;
};

inline uint32_t GetPlatformCapabilities()
{
    return 0;
};

inline uint32_t GetTargetCapabilities()
{
    return 0;
};

inline bool RequestToLaunchProprietaryBootloader()
{
    return false;
};

inline bool RequestToLaunchNanoBooter(int32_t errorCode)
{
    (void)errorCode;

    return false;
};

inline uint32_t CPU_TicksPerSecond()
{
    return 100000000;
}

inline uint64_t CPU_MicrosecondsToTicks(uint64_t uSec)
{
    return uSec * 10;
}

inline uint64_t CPU_MillisecondsToTicks(uint64_t uSec)
{
    return uSec * 10 * 1000;
}

#endif // TARGET_HAL_H
