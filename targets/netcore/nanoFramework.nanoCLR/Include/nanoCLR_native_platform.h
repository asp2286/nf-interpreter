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

#ifndef S_OK
#define S_OK ((HRESULT)0)
#endif

#ifndef E_FAIL
#define E_FAIL ((HRESULT)0x80004005)
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
