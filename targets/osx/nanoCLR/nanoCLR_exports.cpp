//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Stub implementations of the P/Invoke functions declared in
// targets/netcore/nanoFramework.nanoCLR.Host/Interop/nanoCLR.cs
// and targets/netcore/nanoFramework.nanoCLR/nanoCLR_native.h.
//
// These provide the correct C-linkage symbols so the .dylib can be loaded
// by NativeNanoClrLoader. Each stub returns a safe default until the real
// CLR engine is wired up on macOS.

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

// ── Windows-compat typedefs (matches nanoCLR_native.h expectations) ─────────
using HRESULT = int;
using BOOL = int;
using CLR_UINT8 = uint8_t;

static constexpr HRESULT S_OK = 0;
static constexpr HRESULT E_NOTIMPL = static_cast<HRESULT>(0x80004001);

// ── Settings struct (matches managed nanoCLRSettings / NANO_CLR_SETTINGS) ───
struct NANO_CLR_SETTINGS
{
    unsigned short MaxContextSwitches;
    BOOL WaitForDebugger;
    BOOL EnterDebuggerLoopAfterExit;
    BOOL PerformGarbageCollection;
    BOOL PerformHeapCompaction;
};

// ── Callback typedefs ───────────────────────────────────────────────────────
typedef HRESULT (*ConfigureRuntimeCallback)();
typedef void (*DebugPrintCallback)(const char *szText);
typedef void (*ProfilerMessageCallback)(const char *szText);
typedef void (*ProfilerDataCallback)(const CLR_UINT8 *data, size_t size);
typedef int (*WireTransmitCallback)(const CLR_UINT8 *data, size_t size);
typedef int (*WireReceiveCallback)(const CLR_UINT8 *data, size_t size);

// ── Global callback storage ─────────────────────────────────────────────────
static ConfigureRuntimeCallback g_ConfigureCallback = nullptr;
static DebugPrintCallback g_DebugPrintCallback = nullptr;
static WireTransmitCallback g_WireTransmitCallback = nullptr;
static WireReceiveCallback g_WireReceiveCallback = nullptr;
static ProfilerMessageCallback g_ProfilerMessageCallback = nullptr;
static ProfilerDataCallback g_ProfilerDataCallback = nullptr;

// ── Version string ──────────────────────────────────────────────────────────
#ifndef NANOCLR_OSX_VERSION_STRING
#define NANOCLR_OSX_VERSION_STRING "0.1.0"
#endif

// ── Exported functions ──────────────────────────────────────────────────────

extern "C" {

void nanoCLR_Run(NANO_CLR_SETTINGS nanoClrSettings)
{
    (void)nanoClrSettings;
    // TODO: Wire up real CLR startup (ClrStartup) once engine is portable.
}

HRESULT nanoCLR_LoadAssembly(const wchar_t *name, const CLR_UINT8 *data, size_t size)
{
    (void)name; (void)data; (void)size;
    // macOS: not yet implemented. Returning E_NOTIMPL so managed code
    // sees a clear failure rather than silent success.
    return E_NOTIMPL;
}

HRESULT nanoCLR_LoadAssembliesSet(const CLR_UINT8 *data, size_t size)
{
    (void)data; (void)size;
    return E_NOTIMPL;
}

HRESULT nanoCLR_Resolve()
{
    return E_NOTIMPL;
}

void nanoCLR_SetConfigureCallback(ConfigureRuntimeCallback configureCallback)
{
    g_ConfigureCallback = configureCallback;
}

void nanoCLR_SetDebugPrintCallback(DebugPrintCallback debugPrintCallback)
{
    g_DebugPrintCallback = debugPrintCallback;
}

void nanoCLR_WireProtocolOpen()
{
    // TODO: Initialize wire protocol transport.
}

void nanoCLR_WireProtocolClose()
{
    // TODO: Tear down wire protocol transport.
}

void nanoCLR_SetWireProtocolReceiveCallback(WireReceiveCallback receiveCallback)
{
    g_WireReceiveCallback = receiveCallback;
}

void nanoCLR_SetWireProtocolTransmitCallback(WireTransmitCallback transmitCallback)
{
    g_WireTransmitCallback = transmitCallback;
}

void nanoCLR_SetProfilerMessageCallback(ProfilerMessageCallback profilerMessageCallback)
{
    g_ProfilerMessageCallback = profilerMessageCallback;
}

void nanoCLR_SetProfilerDataCallback(ProfilerDataCallback profilerDataCallback)
{
    g_ProfilerDataCallback = profilerDataCallback;
}

void nanoCLR_WireProtocolProcess()
{
    // TODO: Process wire protocol messages.
}

const char *nanoCLR_GetVersion()
{
    // .NET marshals LPStr return values and then calls free() on the pointer,
    // so we must return a malloc-allocated copy.
    static const char version[] = NANOCLR_OSX_VERSION_STRING;
    char *copy = static_cast<char *>(malloc(sizeof(version)));
    memcpy(copy, version, sizeof(version));
    return copy;
}

uint16_t nanoCLR_GetNativeAssemblyCount()
{
    // TODO: Return real native assembly count once engine is wired.
    return 0;
}

bool nanoCLR_GetNativeAssemblyInformation(const CLR_UINT8 *data, size_t size)
{
    (void)data;
    (void)size;
    // TODO: Fill buffer with native assembly metadata.
    return false;
}

} // extern "C"
