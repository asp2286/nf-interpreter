//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifdef _WIN32

#include "stdafx.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}

#else // POSIX (macOS, Linux)

__attribute__((constructor))
static void nanoCLR_library_init(void)
{
    // initialization equivalent to DLL_PROCESS_ATTACH
}

__attribute__((destructor))
static void nanoCLR_library_fini(void)
{
    // cleanup equivalent to DLL_PROCESS_DETACH
}

#endif
