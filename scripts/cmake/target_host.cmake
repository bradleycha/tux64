# ------------------------------------------------------------------------------
#                           Copyright (C) Tux64 2026                   		
# ------------------------------------------------------------------------------
# scripts/cmake/target_host.cmake - CMake target configuration for the host.
# ------------------------------------------------------------------------------

set(CMAKE_C_COMPILER   $ENV{CC})
set(CMAKE_CXX_COMPILER $ENV{CXX})

set(CMAKE_BUILD_TYPE       Release)
set(CMAKE_C_FLAGS          $ENV{TUX64_CFLAGS_HOST})
set(CMAKE_EXE_LINKER_FLAGS $ENV{TUX64_LDFLAGS_HOST})

set(CMAKE_FIND_ROOT_PATH  $ENV{TUX64_BUILD_ROOT}/tools)
set(CMAKE_INSTALL_PREFIX  $ENV{TUX64_BUILD_ROOT}/tools)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
