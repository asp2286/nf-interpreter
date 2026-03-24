#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

cmake_minimum_required(VERSION 3.25)

# ---------------------------------------------------------------------------
# Version handling — BUILD_VERSION comes from CI or caller.
# ---------------------------------------------------------------------------
if(NOT DEFINED BUILD_VERSION)
    set(BUILD_VERSION "9.99.999.9999")
endif()

string(REPLACE "." ";" _VERSION_PARTS "${BUILD_VERSION}")
list(LENGTH _VERSION_PARTS _VERSION_PARTS_LEN)

if(_VERSION_PARTS_LEN GREATER_EQUAL 1)
    list(GET _VERSION_PARTS 0 NBGV_VERSION_MAJOR)
else()
    set(NBGV_VERSION_MAJOR 9)
endif()
if(_VERSION_PARTS_LEN GREATER_EQUAL 2)
    list(GET _VERSION_PARTS 1 NBGV_VERSION_MINOR)
else()
    set(NBGV_VERSION_MINOR 99)
endif()
if(_VERSION_PARTS_LEN GREATER_EQUAL 3)
    list(GET _VERSION_PARTS 2 NBGV_BUILD_NUMBER)
else()
    set(NBGV_BUILD_NUMBER 999)
endif()
if(_VERSION_PARTS_LEN GREATER_EQUAL 4)
    list(GET _VERSION_PARTS 3 TARGET_BUILD_COUNTER)
else()
    set(TARGET_BUILD_COUNTER 9999)
endif()

project(nanoFramework_nanoCLR_native
    VERSION "${NBGV_VERSION_MAJOR}.${NBGV_VERSION_MINOR}.${NBGV_BUILD_NUMBER}"
    LANGUAGES C CXX
)

# ---------------------------------------------------------------------------
# Repository root — all src/** and targets/** paths resolve from here.
# ---------------------------------------------------------------------------
get_filename_component(REPO_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../.." ABSOLUTE)

# ---------------------------------------------------------------------------
# littlefs — external clone required on all platforms.
# The vcxproj uses the LITTLEFS_PATH environment variable.
# ---------------------------------------------------------------------------
if(NOT DEFINED LITTLEFS_PATH AND DEFINED ENV{LITTLEFS_PATH})
    set(LITTLEFS_PATH "$ENV{LITTLEFS_PATH}")
endif()
if(NOT DEFINED LITTLEFS_PATH)
    message(WARNING
        "LITTLEFS_PATH is not set. "
        "Set -DLITTLEFS_PATH=<path> to a littlefs repository clone. "
        "Build will proceed but littlefs sources will not be found.")
endif()

# ---------------------------------------------------------------------------
# Source files — faithfully mirrors the vcxproj <ClCompile> items.
# ---------------------------------------------------------------------------

# Sources local to targets/netcore/nanoFramework.nanoCLR/
set(NANOCLR_NETCORE_LOCAL_SOURCES
    CLRStartup.cpp
    Device_BlockStorage.cpp
    dllmain.cpp
    nanoCLR_native.cpp
    platform_BlockStorage.cpp
    Target_BlockStorage.cpp
    Target_BlockStorage_TransientFlashDriver.cpp
    target_FileSystem.cpp
    Target_HAL.cpp
    target_littlefs.cpp
    WireProtocol_HAL_Interface.cpp
)

# stdafx.cpp (PCH creator) — Windows/MSVC only.
if(WIN32)
    list(APPEND NANOCLR_NETCORE_LOCAL_SOURCES
        stdafx.cpp
    )
endif()

# Sources reused from targets/win32/nanoCLR/ (vcxproj references them
# via relative paths like ..\..\win32\nanoCLR\<file>).
set(NANOCLR_WIN32_SHARED_SOURCES
    ${REPO_ROOT}/targets/win32/nanoCLR/Various.cpp
    ${REPO_ROOT}/targets/win32/nanoCLR/base64.cpp
    ${REPO_ROOT}/targets/win32/nanoCLR/targetPAL_Events.cpp
    ${REPO_ROOT}/targets/win32/nanoCLR/FileStore_Win32.cpp
    ${REPO_ROOT}/targets/win32/nanoCLR/Generated/CLR_RT_InteropAssembliesTable.cpp
    ${REPO_ROOT}/targets/win32/nanoCLR/Memory.cpp
    ${REPO_ROOT}/targets/win32/nanoCLR/PAL/COMDirector_stubs.cpp
    ${REPO_ROOT}/targets/win32/nanoCLR/PAL/ConfigHelper_stubs.cpp
    ${REPO_ROOT}/targets/win32/nanoCLR/PAL/time_stubs.cpp
    ${REPO_ROOT}/targets/win32/nanoCLR/PAL/Watchdog_stubs.cpp
    ${REPO_ROOT}/targets/win32/nanoCLR/platform_heap.cpp
    ${REPO_ROOT}/targets/win32/nanoCLR/targetPAL_Time.cpp
    ${REPO_ROOT}/targets/win32/nanoCLR/targetRandom.cpp
    ${REPO_ROOT}/targets/win32/nanoCLR/targetHAL_Time.cpp
    ${REPO_ROOT}/targets/win32/nanoCLR/WatchDog.cpp
)

# Sources from src/ compiled directly (not via static-lib project references).
set(NANOCLR_SRC_SOURCES
    ${REPO_ROOT}/src/PAL/FileSystem/nanoPAL_FileSystem.cpp
    ${REPO_ROOT}/src/CLR/Core/NativeEventDispatcher/NativeEventDispatcher.cpp
    ${REPO_ROOT}/src/HAL/nanoHAL_Time.cpp
    ${REPO_ROOT}/src/nanoFramework.Runtime.Events/nf_rt_events_native.cpp
    ${REPO_ROOT}/src/nanoFramework.Runtime.Events/nf_rt_events_native_nanoFramework_Runtime_Events_EventSink.cpp
    ${REPO_ROOT}/src/nanoFramework.Runtime.Events/nf_rt_events_native_nanoFramework_Runtime_Events_NativeEventDispatcher.cpp
    ${REPO_ROOT}/src/nanoFramework.Runtime.Events/nf_rt_events_native_nanoFramework_Runtime_Events_WeakDelegate.cpp
    ${REPO_ROOT}/src/PAL/AsyncProcCall/AsyncCompletions.cpp
    ${REPO_ROOT}/src/PAL/AsyncProcCall/AsyncContinuations.cpp
    ${REPO_ROOT}/src/PAL/Double/nanoPAL_NativeDouble.cpp
)

# ---------------------------------------------------------------------------
# SHARED library target — produces:
#   Windows: nanoFramework.nanoCLR.dll
#   macOS:   nanoFramework.nanoCLR.dylib
#   Linux:   nanoFramework.nanoCLR.so
# ---------------------------------------------------------------------------
add_library(nanoFramework.nanoCLR SHARED
    ${NANOCLR_NETCORE_LOCAL_SOURCES}
    ${NANOCLR_WIN32_SHARED_SOURCES}
    ${NANOCLR_SRC_SOURCES}
)

set_target_properties(nanoFramework.nanoCLR PROPERTIES
    OUTPUT_NAME "nanoFramework.nanoCLR"
    PREFIX ""                                         # no "lib" prefix on any platform
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib" # DLLs go to RUNTIME on Windows
)

# ---------------------------------------------------------------------------
# C++ / C standard — matches nf.props: stdcpp20 / stdc17
# ---------------------------------------------------------------------------
target_compile_features(nanoFramework.nanoCLR PRIVATE cxx_std_20)
set_target_properties(nanoFramework.nanoCLR PROPERTIES
    C_STANDARD 17
    C_STANDARD_REQUIRED ON
)

# ---------------------------------------------------------------------------
# Include directories — union of vcxproj + nf.props, de-duplicated.
# ---------------------------------------------------------------------------
target_include_directories(nanoFramework.nanoCLR PRIVATE
    # Local / platform headers
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
    ${REPO_ROOT}/targets/win32/Include

    # netcore self-reference (nf.props adds this at multiple depths)
    ${REPO_ROOT}/targets/netcore/nanoFramework.nanoCLR

    # CLR / HAL / PAL / Runtime headers
    ${REPO_ROOT}/src/CLR/Core
    ${REPO_ROOT}/src/CLR/CorLib
    ${REPO_ROOT}/src/CLR/Helpers/Base64
    ${REPO_ROOT}/src/CLR/Include
    ${REPO_ROOT}/src/HAL/Include
    ${REPO_ROOT}/src/PAL/Include
    ${REPO_ROOT}/src/nanoFramework.Runtime.Events

    # littlefs — local config header + external repo
    ${REPO_ROOT}/targets/netcore/littlefs
)

if(DEFINED LITTLEFS_PATH)
    target_include_directories(nanoFramework.nanoCLR PRIVATE
        ${LITTLEFS_PATH}
    )
endif()

# ---------------------------------------------------------------------------
# Preprocessor definitions — vcxproj + nf.props, config-aware.
# ---------------------------------------------------------------------------
target_compile_definitions(nanoFramework.nanoCLR PRIVATE
    # From nf.props — always set
    VIRTUAL_DEVICE
    SUPPORT_ANY_BASE_CONVERSION
    NANOCLR_REFLECTION=TRUE
    NANOCLR_SYSTEM_COLLECTIONS=TRUE
    _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
    VERSION_MAJOR=${NBGV_VERSION_MAJOR}
    VERSION_MINOR=${NBGV_VERSION_MINOR}
    VERSION_BUILD=${NBGV_BUILD_NUMBER}
    VERSION_REVISION=${TARGET_BUILD_COUNTER}
    $<IF:$<PLATFORM_ID:Windows>,PLATFORMNAMESTRING="WINDOWS",PLATFORMNAMESTRING="OSX">

    # From vcxproj — DLL export macro
    NANOCLRNATIVE_EXPORTS
    _CONSOLE
    VERSION_STRING="${BUILD_VERSION}"

    # Config-dependent
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Debug>:DEBUG>
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
)

# ---------------------------------------------------------------------------
# Platform-specific compiler/linker flags
# ---------------------------------------------------------------------------
if(MSVC)
    target_compile_options(nanoFramework.nanoCLR PRIVATE
        /W3            # WarningLevel = Level3
        /EHsc          # C++ exception handling
        /utf-8         # Unicode charset
        $<$<NOT:$<CONFIG:Debug>>:/Gy>   # FunctionLevelLinking
        $<$<NOT:$<CONFIG:Debug>>:/Oi>   # IntrinsicFunctions
        $<$<NOT:$<CONFIG:Debug>>:/GL>   # WholeProgramOptimization
    )

    target_link_options(nanoFramework.nanoCLR PRIVATE
        /SUBSYSTEM:WINDOWS              # vcxproj uses Windows subsystem (not Console)
        $<$<NOT:$<CONFIG:Debug>>:/OPT:ICF>   # EnableCOMDATFolding
        $<$<NOT:$<CONFIG:Debug>>:/OPT:REF>   # OptimizeReferences
        $<$<NOT:$<CONFIG:Debug>>:/LTCG>      # Link-time code generation
        $<$<CONFIG:Debug>:/INCREMENTAL>      # Debug incremental linking
    )

    # Precompiled header — stdafx.h / stdafx.cpp
    target_precompile_headers(nanoFramework.nanoCLR PRIVATE stdafx.h)
endif()

if(APPLE)
    # Symbols are exported via extern "C" / __attribute__((visibility("default")))
    # CMake default visibility for SHARED is already correct on macOS.
    set_target_properties(nanoFramework.nanoCLR PROPERTIES
        MACOSX_RPATH ON
    )
endif()

# ---------------------------------------------------------------------------
# Library dependencies
#
# The vcxproj uses ProjectReferences to static-library .vcxproj projects
# and #pragma comment(lib, ...) in nanoCLR_native.cpp.
# When those libraries are ported to CMake targets, link them here.
# ---------------------------------------------------------------------------

# On Windows, kernel32 is linked via #pragma comment(lib, ...) in stdafx.h.
# Make it explicit for CMake:
if(WIN32)
    target_link_libraries(nanoFramework.nanoCLR PRIVATE kernel32)
endif()

# Project references that ARE linked (LinkLibraryDependencies != false):
# TODO: Replace with CMake target names once each library has a CMakeLists.txt
#   - Core              (src/CLR/Core/Core.vcxproj)
#   - FileStream        (src/CLR/Core/FileStream/FileStream.vcxproj)           ** netcore-only **
#   - Hardware          (src/CLR/Core/Hardware/Hardware.vcxproj)
#   - InterruptHandler  (src/CLR/Core/InterruptHandler/InterruptHandler.vcxproj)
#   - RPC               (src/CLR/Core/RPC/RPC.vcxproj)
#   - Serialization     (src/CLR/Core/Serialization/Serialization.vcxproj)
#   - CorLib            (src/CLR/CorLib/CorLib.vcxproj)
#   - Debugger          (src/CLR/Debugger/Debugger.vcxproj)
#   - Diagnostics       (src/CLR/Diagnostics/Diagnostics.vcxproj)
#   - Messaging         (src/CLR/Messaging/Messaging.vcxproj)
#   - StartUp           (src/CLR/StartUp/StartUp.vcxproj)
#   - System.Math       (src/CLR/System.Math/System.Math.vcxproj)
#   - WireProtocol      (src/CLR/WireProtocol/WireProtocol.vcxproj)
#   - Networking.Sntp   (src/DeviceInterfaces/Networking.Sntp/Networking.Sntp.vcxproj)
#   - System.Net        (src/DeviceInterfaces/System.Net/System.Net.vcxproj)
#   - nanoFramework.ResourceManager
#   - Runtime.Native
#   - System.Collections
#   - System.IO.Hashing
#   - System.Text
#   - System.Device.Gpio
#   - System.Device.I2c
#   - System.Device.Spi
#   - System.IO.FileSystem  (src/System.IO.FileSystem/System.IO.FileSystem.vcxproj)  ** netcore-only **
#   - System.IO.Ports
#   - System.Runtime.Serialization
#   - littlefs           (targets/netcore/littlefs/littlefs.vcxproj)              ** netcore-only **

# Stubs referenced but NOT linked (LinkLibraryDependencies=false):
#   - InterruptHandler_stub
#   - NativeEventDispatcher_stub
#   - RPC_stub
#   - Debugger_stub
#   - Diagnostics_stub
#   - Messaging_stub
