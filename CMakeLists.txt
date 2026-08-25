cmake_minimum_required(VERSION 3.21)

project(
  NeQuick2MLF2
  VERSION 1.1
  DESCRIPTION
    "Ionospheric Model NeQuick2-MLF2 designed by Gustave Eiffel University"
  LANGUAGES CXX C)

if(NOT WIN32)
  set(CMAKE_CXX_COMPILER
      /usr/bin/g++-11
      CACHE FILEPATH "GNU C++ compiler")
  add_compile_options(-fPIC)
endif()

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
    ${CMAKE_SOURCE_DIR}/bin/x64/${CMAKE_BUILD_TYPE}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
    ${CMAKE_SOURCE_DIR}/bin/x64/${CMAKE_BUILD_TYPE})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${CMAKE_SOURCE_DIR}/bin/x64/${CMAKE_BUILD_TYPE})

# Force the variable to not be null
if(NOT CMAKE_BUILD_TYPE STREQUAL Debug)
  set(CMAKE_BUILD_TYPE Release)
endif()

include(cmake/tpl.cmake)

add_subdirectory(src)
