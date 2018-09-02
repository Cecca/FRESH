cmake_minimum_required(VERSION 3.0)
project(curvedist)

#include_directories(/usr/local/Cellar/llvm/6.0.0/include)

# Default to Release mode
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING
      "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif(NOT CMAKE_BUILD_TYPE)

# Always color output
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  add_compile_options (-fdiagnostics-color=always)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  add_compile_options (-fcolor-diagnostics)
endif ()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

# Git information

include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC GIT_SHA1)
git_describe(GIT_DESCRIBE)

# Dependencies

#find_package(OpenMP REQUIRED)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED
  COMPONENTS system filesystem iostreams timer chrono program_options date_time)

find_package(BZip2)
if( BZIP2_FOUND )
  add_definitions(-DEXPERIMENT_COMPRESS=1)
else()
  message("-- BZip2 not found, experimental results will not be compressed")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -Wall -std=c++11")

include_directories(include)

# Subprojects

add_subdirectory(core)
add_subdirectory(test)
add_subdirectory(utils)
