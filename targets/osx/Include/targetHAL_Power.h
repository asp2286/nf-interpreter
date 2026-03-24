//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef TARGET_HAL_POWER_H
#define TARGET_HAL_POWER_H

#include <cstdlib>

inline void CPU_Reset()
{
    // In shared library mode the host process owns lifetime.
    // Signal an abnormal condition rather than forcing exit.
    // TODO: wire into host shutdown callback when runtime is integrated.
    std::abort();
}

inline bool CPU_IsSoftRebootSupported()
{
    // Soft reboot is not supported in the current scaffold.
    return false;
}

#endif // TARGET_HAL_POWER_H
