#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# ---------------------------------------------------------------------------
# Repository root — all src/** paths are relative to this.
# ---------------------------------------------------------------------------
get_filename_component(REPO_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../.." ABSOLUTE)

# ---------------------------------------------------------------------------
# Source files compiled directly by the vcxproj
# (project-reference static libraries are listed under target_link_libraries)
# ---------------------------------------------------------------------------

# Sources local to targets/win32/nanoCLR/
set(NANOCLR_WIN32_LOCAL_SOURCES
    CLRStartup.cpp
    FileStore_Win32.cpp
    Memory.cpp
    base64.cpp
    main.cpp
    platform_heap.cpp
    stdafx.cpp
    targetHAL_Time.cpp
    targetPAL_Events.cpp
    targetPAL_Time.cpp
    targetRandom.cpp
    Target_BlockStorage.cpp
    Target_HAL.cpp
    Various.cpp
    WatchDog.cpp
    ParseOptions_Win32.cpp
    Generated/CLR_RT_InteropAssembliesTable.cpp
    PAL/blockstorageList_stubs.cpp
    PAL/COMDirector_stubs.cpp
    PAL/ConfigHelper_stubs.cpp
    PAL/time_stubs.cpp
    PAL/Watchdog_stubs.cpp
)

# Sources from src/ compiled directly (not via static-lib project references).
set(NANOCLR_WIN32_SRC_SOURCES
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
# EXE target — matches the existing .vcxproj (ConfigurationType=Application)
# ---------------------------------------------------------------------------
add_executable(nanoFramework.nanoCLR
    ${NANOCLR_WIN32_LOCAL_SOURCES}
    ${NANOCLR_WIN32_SRC_SOURCES}
)

set_target_properties(nanoFramework.nanoCLR PROPERTIES
    OUTPUT_NAME "nanoFramework.nanoCLR"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# ---------------------------------------------------------------------------
# C++ / C standard  (matches nf.props: stdcpp20 / stdc17)
# ---------------------------------------------------------------------------
target_compile_features(nanoFramework.nanoCLR PRIVATE cxx_std_20)
set_target_properties(nanoFramework.nanoCLR PROPERTIES C_STANDARD 17 C_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# Include directories  (union of vcxproj + nf.props, de-duplicated)
# ---------------------------------------------------------------------------
target_include_directories(nanoFramework.nanoCLR PRIVATE
    # Local / platform headers
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../Include

    # netcore shared header (referenced by nf.props)
    ${REPO_ROOT}/targets/netcore/nanoFramework.nanoCLR

    # CLR / HAL / PAL / Runtime headers
    ${REPO_ROOT}/src/CLR/Core
    ${REPO_ROOT}/src/CLR/CorLib
    ${REPO_ROOT}/src/CLR/Helpers/Base64
    ${REPO_ROOT}/src/CLR/Include
    ${REPO_ROOT}/src/HAL/Include
    ${REPO_ROOT}/src/PAL/Include
    ${REPO_ROOT}/src/nanoFramework.Runtime.Events
)

# ---------------------------------------------------------------------------
# Preprocessor definitions  (vcxproj + nf.props, config-aware)
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
    PLATFORMNAMESTRING="WINDOWS"

    # From vcxproj — always set
    NANOWIN32
    _CONSOLE
    VERSION_STRING="${BUILD_VERSION}"

    # Config-dependent (CMake maps $<CONFIG:Debug> to VS Debug)
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Debug>:DEBUG>
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
)

# ---------------------------------------------------------------------------
# Compiler flags — MSVC-specific, matching the .vcxproj settings
# ---------------------------------------------------------------------------
if(MSVC)
    target_compile_options(nanoFramework.nanoCLR PRIVATE
        /W3            # WarningLevel = Level3
        /EHsc          # C++ exception handling (implicit in MSVC projects)
        /utf-8         # CharacterSet = Unicode
        # Release optimizations are handled by CMake's default CMAKE_CXX_FLAGS_RELEASE (/O2)
        # FunctionLevelLinking (/Gy) and IntrinsicFunctions (/Oi) for Release
        $<$<NOT:$<CONFIG:Debug>>:/Gy>
        $<$<NOT:$<CONFIG:Debug>>:/Oi>
        # WholeProgramOptimization (/GL) for Release
        $<$<NOT:$<CONFIG:Debug>>:/GL>
    )

    # Linker flags matching vcxproj
    target_link_options(nanoFramework.nanoCLR PRIVATE
        /SUBSYSTEM:CONSOLE
        # Release: EnableCOMDATFolding + OptimizeReferences
        $<$<NOT:$<CONFIG:Debug>>:/OPT:ICF>
        $<$<NOT:$<CONFIG:Debug>>:/OPT:REF>
        # Release: Link-time code generation (pairs with /GL)
        $<$<NOT:$<CONFIG:Debug>>:/LTCG>
        # Debug: incremental linking
        $<$<CONFIG:Debug>:/INCREMENTAL>
    )
endif()

# ---------------------------------------------------------------------------
# Precompiled header  (stdafx.h / stdafx.cpp — mirrors vcxproj PCH setup)
# ---------------------------------------------------------------------------
if(MSVC)
    target_precompile_headers(nanoFramework.nanoCLR PRIVATE stdafx.h)
endif()

# ---------------------------------------------------------------------------
# Library dependencies
#
# The vcxproj uses ProjectReferences to static-library .vcxproj projects.
# When those libraries are ported to CMake targets, link them here.
# For now, list the library search directory and the expected target names
# as comments so nothing is silently lost.
# ---------------------------------------------------------------------------

# Additional library directory from vcxproj ($(BinDir)):
# link_directories("${CMAKE_BINARY_DIR}/bin")

# Project references that ARE linked (LinkLibraryDependencies != false):
# TODO: Replace with CMake target names once each library has a CMakeLists.txt
#   - Core            (src/CLR/Core/Core.vcxproj)
#   - Hardware        (src/CLR/Core/Hardware/Hardware.vcxproj)
#   - InterruptHandler(src/CLR/Core/InterruptHandler/InterruptHandler.vcxproj)
#   - RPC             (src/CLR/Core/RPC/RPC.vcxproj)
#   - Serialization   (src/CLR/Core/Serialization/Serialization.vcxproj)
#   - CorLib          (src/CLR/CorLib/CorLib.vcxproj)
#   - Debugger        (src/CLR/Debugger/Debugger.vcxproj)
#   - Diagnostics     (src/CLR/Diagnostics/Diagnostics.vcxproj)
#   - Messaging       (src/CLR/Messaging/Messaging.vcxproj)
#   - StartUp         (src/CLR/StartUp/StartUp.vcxproj)
#   - System.Math     (src/CLR/System.Math/System.Math.vcxproj)
#   - WireProtocol    (src/CLR/WireProtocol/WireProtocol.vcxproj)
#   - Networking.Sntp (src/DeviceInterfaces/Networking.Sntp/Networking.Sntp.vcxproj)
#   - System.Net      (src/DeviceInterfaces/System.Net/System.Net.vcxproj)
#   - nanoFramework.ResourceManager
#   - Runtime.Native
#   - System.Collections
#   - System.IO.Hashing
#   - System.Text
#   - System.Device.Gpio
#   - System.Device.I2c
#   - System.Device.Spi
#   - System.IO.Ports
#   - System.Runtime.Serialization

# Stubs referenced but NOT linked (LinkLibraryDependencies=false):
#   - InterruptHandler_stub
#   - NativeEventDispatcher_stub
#   - RPC_stub
#   - Debugger_stub
#   - Diagnostics_stub
#   - Messaging_stub
