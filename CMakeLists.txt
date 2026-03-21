cmake_minimum_required (VERSION 3.25.0)

set(CMAKE_TOOLCHAIN_FILE "E:/vcpkg/scripts/buildsystems/vcpkg.cmake")

option(BUILD_TESTING  ON)

project ("ecs" LANGUAGES CXX C)
set(CMAKE_CXX_EXCEPTIONS OFF)
add_compile_definitions(PROJECT_DIR="${CMAKE_SOURCE_DIR}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/dist)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/dist)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/dist/lib)
add_compile_definitions(_UTF8)



set(CMAKE_CXX_SCAN_FOR_MODULES ON)
add_subdirectory("thirdparty")


include(source.cmake)

set(BUILD_TESTING ON)
add_subdirectory ("ecs")
add_subdirectory("renderer")
add_subdirectory ("game")


if(BUILD_TESTING)
    add_subdirectory(test)
endif ()

 
