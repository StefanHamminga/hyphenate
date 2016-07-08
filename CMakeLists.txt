cmake_minimum_required (VERSION 3.1.0 FATAL_ERROR)

project (hyphenate)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")

set(hyphenate_SRCS src/hyphenate.cpp)
add_executable(hyphenate ${hyphenate_SRCS})

set (CMAKE_CXX_STANDARD 14)

#set (CMAKE_CURRENT_BINARY_DIR ${PROJECT_SOURCE_DIR}/bin)
set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set (hyphenate_VERSION_MAJOR 1)
set (hyphenate_VERSION_MINOR 0)
set (hyphenate_VERSION_PATCH 0)

configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
  )

include_directories("${PROJECT_BINARY_DIR}")

find_library(HYPHEN hyphen)
target_link_libraries(hyphenate ${HYPHEN})

install (TARGETS hyphenate DESTINATION bin)
