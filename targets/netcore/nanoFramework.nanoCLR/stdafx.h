//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

// Cross-platform compatibility layer (must come first — defines HRESULT, BOOL, etc. on POSIX)
#include "Include/nanoCLR_native_platform.h"

#ifdef _WIN32
#include "targetver.h"
#include <tchar.h>
#include <crtdbg.h>
#pragma comment(lib, "kernel32.lib")
#endif

#include <stdio.h>
#include <mutex>

// TODO: reference additional headers your program requires here

#include <nanoCLR_Runtime.h>
#include <nanoCLR_Hardware.h>
#include <nanoCLR_Application.h>
#include <base64.h>

#include "nanoCLR_ParseOptions.h"
