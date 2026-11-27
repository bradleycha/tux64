# ------------------------------------------------------------------------------
#                           Copyright (C) Tux64 2026                   		
# ------------------------------------------------------------------------------
# scripts/cmake/target_n64_linux.cmake - CMake target configuration for Linux
#    running on the Nintendo 64.
# ------------------------------------------------------------------------------

set(CMAKE_SYSTEM_NAME      Linux)
set(CMAKE_SYSTEM_PROCESSOR Mips)

set(CMAKE_C_COMPILER   $ENV{CC})
set(CMAKE_CXX_COMPILER $ENV{CXX})

set(CMAKE_BUILD_TYPE       MinSizeRel)
set(CMAKE_C_FLAGS          $ENV{TUX64_CFLAGS_N64_LINUX})
set(CMAKE_EXE_LINKER_FLAGS $ENV{TUX64_LDFLAGS_N64_LINUX})

set(CMAKE_FIND_ROOT_PATH  $ENV{TUX64_BUILD_ROOT}/tools/$ENV{TUX64_TARGET_N64_LINUX})
set(CMAKE_INSTALL_PREFIX  $ENV{TUX64_BUILD_ROOT}/tools/$ENV{TUX64_TARGET_N64_LINUX})

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
