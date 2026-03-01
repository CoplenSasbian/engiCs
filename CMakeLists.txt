cmake_minimum_required (VERSION 3.25.0)

set(CMAKE_TOOLCHAIN_FILE "E:/vcpkg/scripts/buildsystems/vcpkg.cmake")

option(BUILD_TESTING  ON)

project ("ecs" LANGUAGES CXX C)

add_compile_definitions(PROJECT_DIR="${CMAKE_SOURCE_DIR}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/dist)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/dist)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/dist/lib)

set(CMAKE_CXX_SCAN_FOR_MODULES ON)
add_subdirectory("thirdparty")


set(BUILD_TESTING ON)
add_subdirectory ("ecs")
 
