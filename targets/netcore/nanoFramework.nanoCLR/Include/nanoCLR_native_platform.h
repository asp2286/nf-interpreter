//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
// Cross-platform compatibility layer for nanoCLR native library.
// Windows: includes the real Win32 definitions.
// POSIX (macOS, Linux): provides portable equivalents.
//

#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cstdlib>

// ---------------------------------------------------------------------------
// Symbol export / import
// ---------------------------------------------------------------------------
#ifdef _WIN32
#define NANOCLR_EXPORT __declspec(dllexport)
#define NANOCLR_IMPORT __declspec(dllimport)
#else
#define NANOCLR_EXPORT __attribute__((visibility("default")))
#define NANOCLR_IMPORT
#endif

#ifdef NANOCLRNATIVE_EXPORTS
#define NANOCLR_API NANOCLR_EXPORT
#else
#define NANOCLR_API NANOCLR_IMPORT
#endif

// ---------------------------------------------------------------------------
// Calling convention — __stdcall is MSVC-only; no-op on other platforms.
// ---------------------------------------------------------------------------
#ifndef _WIN32
#ifndef __stdcall
#define __stdcall
#endif
#endif

// ---------------------------------------------------------------------------
// Win32 type compatibility
// ---------------------------------------------------------------------------
#ifndef _WIN32

#ifndef HRESULT
typedef int32_t HRESULT;
#endif

#ifndef S_FALSE
#define S_FALSE ((HRESULT)1)
#endif

#ifndef S_OK
#define S_OK ((HRESULT)0)
#endif

#ifndef E_FAIL
#define E_FAIL ((HRESULT)0x80004005)
#endif

#ifndef SUCCEEDED
#define SUCCEEDED(Status) ((HRESULT)(Status) >= 0)
#endif

#ifndef FAILED
#define FAILED(Status) ((HRESULT)(Status) < 0)
#endif

#ifndef HRESULT_CODE
#define HRESULT_CODE(hr) ((hr)&0xFFFF)
#endif

#ifndef HRESULT_FACILITY
#define HRESULT_FACILITY(hr) (((hr) >> 16) & 0x1fff)
#endif

#ifndef HRESULT_SEVERITY
#define HRESULT_SEVERITY(hr) (((hr) >> 31) & 0x1)
#endif

#ifndef CLR_UINT8
typedef uint8_t CLR_UINT8;
#endif

#ifndef CLR_UINT16
typedef uint16_t CLR_UINT16;
#endif

#ifndef CLR_UINT32
typedef uint32_t CLR_UINT32;
#endif

#ifndef BOOL
typedef int BOOL;
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef DWORD
typedef uint32_t DWORD;
#endif

#ifndef LONG
typedef int32_t LONG;
#endif

#ifndef ULONG
typedef uint32_t ULONG;
#endif

#ifndef WORD
typedef uint16_t WORD;
#endif

#ifndef BYTE
typedef uint8_t BYTE;
#endif

#ifndef UINT
typedef uint32_t UINT;
#endif

#ifndef UINT64
typedef uint64_t UINT64;
#endif

#ifndef LPCSTR
typedef const char *LPCSTR;
#endif

#ifndef LPCWSTR
typedef const wchar_t *LPCWSTR;
#endif

#ifndef SEVERITY_SUCCESS
#define SEVERITY_SUCCESS 0
#endif

#ifndef SEVERITY_ERROR
#define SEVERITY_ERROR 1
#endif

#ifndef MAKE_HRESULT
#define MAKE_HRESULT(sev, fac, code) \
    ((HRESULT)(((unsigned long)(sev) << 31) | ((unsigned long)(fac) << 16) | ((unsigned long)(code))))
#endif

#ifndef __int8
#define __int8 char
#endif

#ifndef __int16
#define __int16 short
#endif

#ifndef __int32
#define __int32 int
#endif

#ifndef __int64
#define __int64 long long
#endif

#endif // !_WIN32

// ---------------------------------------------------------------------------
// Cross-platform wrappers for Win32 APIs used in nanoCLR_native.cpp
// ---------------------------------------------------------------------------
#ifdef _WIN32

#include <Windows.h>

#define NANOCLR_ZERO_MEMORY(ptr, size) ZeroMemory(ptr, size)
#define NANOCLR_ALLOC(size)            CoTaskMemAlloc(size)
#define NANOCLR_GET_PID()              static_cast<DWORD>(GetCurrentProcessId())

#else // POSIX

#include <unistd.h>

#define NANOCLR_ZERO_MEMORY(ptr, size) memset(ptr, 0, size)
#define NANOCLR_ALLOC(size)            malloc(size)
#define NANOCLR_GET_PID()              static_cast<uint32_t>(getpid())

#endif
