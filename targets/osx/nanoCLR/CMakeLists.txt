#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# Sources shared between the smoke executable and the shared library.
set(NANOCLR_OSX_COMMON_SOURCES
    CLRStartup.cpp
    FileStore_OSX.cpp
    Memory.cpp

    platform_heap.cpp
    targetHAL_Time.cpp
    targetPAL_Events.cpp
    targetPAL_Time.cpp
    targetRandom.cpp
    Target_BlockStorage.cpp
    Target_HAL.cpp
    Various.cpp
    WatchDog.cpp
)

set(NANOCLR_OSX_COMMON_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../Include
)

set(NANOCLR_OSX_COMMON_DEFINITIONS
    NANOCLR_HOST_POSIX=1
    NANOCLR_HOST_OSX=1
    NANOCLR_HOST_WINDOWS=0
    NANOCLR_OSX_VERSION_STRING="${PROJECT_VERSION}"
)

# ── Smoke executable ────────────────────────────────────────────────────────
add_executable(nanoFramework.nanoCLR.Exe
    main.cpp
    ${NANOCLR_OSX_COMMON_SOURCES}
)

target_compile_features(nanoFramework.nanoCLR.Exe PRIVATE cxx_std_20)

target_include_directories(
    nanoFramework.nanoCLR.Exe
    PRIVATE
        ${NANOCLR_OSX_COMMON_INCLUDES}
)

target_compile_definitions(
    nanoFramework.nanoCLR.Exe
    PRIVATE
        ${NANOCLR_OSX_COMMON_DEFINITIONS}
        NANO_OSX_ENABLE_SMOKE=$<BOOL:${NANO_OSX_ENABLE_SMOKE}>
)

set_target_properties(
    nanoFramework.nanoCLR.Exe
    PROPERTIES
        OUTPUT_NAME "nanoFramework.nanoCLR"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# ── Shared library (.dylib) for P/Invoke ────────────────────────────────────
add_library(nanoFramework.nanoCLR SHARED
    nanoCLR_exports.cpp
    ${NANOCLR_OSX_COMMON_SOURCES}
)

target_compile_features(nanoFramework.nanoCLR PRIVATE cxx_std_20)

target_include_directories(
    nanoFramework.nanoCLR
    PRIVATE
        ${NANOCLR_OSX_COMMON_INCLUDES}
)

target_compile_definitions(
    nanoFramework.nanoCLR
    PRIVATE
        ${NANOCLR_OSX_COMMON_DEFINITIONS}
)

# Output: nanoFramework.nanoCLR.dylib (no "lib" prefix).
set_target_properties(
    nanoFramework.nanoCLR
    PROPERTIES
        OUTPUT_NAME "nanoFramework.nanoCLR"
        PREFIX ""
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

