# General project settings
project(hwloc-test)
cmake_minimum_required(VERSION 2.8)

# Enable support for external projects
include(ExternalProject)

########
# Handle different build types
IF(NOT CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE Debug
       CACHE STRING "Choose the type of build : Debug Release."
       FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)
message("* Current build type is : ${CMAKE_BUILD_TYPE}")
string( TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE )
########

########
# Compiler/linker options based on the different compilers
ADD_DEFINITIONS(-std=c++14 )
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  # Using Clang
  if ("${CMAKE_BUILD_TYPE}" STREQUAL "release")
    ADD_DEFINITIONS(-g -Ofast -march=native)
  endif()

  if ("${CMAKE_BUILD_TYPE}" STREQUAL "debug")
    ADD_DEFINITIONS(-g3 -O0 -Weverything)
  endif()

  # Reduce the noise coming from Qt5
  ADD_DEFINITIONS(-Wno-inconsistent-missing-override -Wno-disabled-macro-expansion -Wno-deprecated)

  # Options required to reduce the noise of spdlog
  ADD_DEFINITIONS(-Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-sign-conversion -Wno-padded -Wno-switch-enum)
  ADD_DEFINITIONS(-Wno-old-style-cast -Wno-undef -Wno-documentation-unknown-command)
  ADD_DEFINITIONS(-Wno-weak-vtables -Wno-global-constructors -Wno-exit-time-destructors)

  # Reduce the noise coming from hwloc
  ADD_DEFINITIONS(-Wno-reserved-id-macro -Wno-documentation)

elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  # Using GCC
  if ("${CMAKE_BUILD_TYPE}" STREQUAL "release")
    ADD_DEFINITIONS(-g -O2 -march=native)
  endif()

  if ("${CMAKE_BUILD_TYPE}" STREQUAL "debug")
    ADD_DEFINITIONS(-g3 -O0 -Wall -Wextra -Wunused)
  endif()

elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
  # Using Intel C++
  if ("${CMAKE_BUILD_TYPE}" STREQUAL "release")
    ADD_DEFINITIONS(-g -fast)
  endif()

  if ("${CMAKE_BUILD_TYPE}" STREQUAL "debug")
    ADD_DEFINITIONS(-g3 -O0 -Wall -Wextra)
  endif()

endif()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "release")
  # Strip the binary
  set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s")
endif()
########

########
# hwloc
ExternalProject_Add(hwloc
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/hwloc-1.11.0
    CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/vendor/hwloc-1.11.0/configure --disable-pci --disable-opencl --disable-cuda --disable-nvml --disable-gl --disable-libudev --enable-static --prefix=<INSTALL_DIR> CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER}
    BUILD_IN_SOURCE 0
    BUILD_COMMAND ${MAKE})

# Store c-ares install directory in variable
ExternalProject_Get_Property(hwloc install_dir)
set (hwloc_path ${install_dir})
########

########
# Includes
include_directories(.)

# libcares
include_directories(${hwloc_path}/include)
########

########
# Compiling and linking
add_executable(test work_tree.cpp)
target_link_libraries(test -lnuma)

add_dependencies (test hwloc)
target_link_libraries(test ${hwloc_path}/lib/libhwloc.a)
########
