
add_compile_definitions(_UTF8)

find_package(mimalloc CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(lua CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS  container asio system)

find_path(SQLITE_MODERN_CPP_INCLUDE_DIRS "sqlite_modern_cpp.h")
find_path(PLF_HIVE_INCLUDE_DIRS "plf_hive.h")
find_path(SOL2_INCLUDE_DIRS "sol/sol.hpp")

include(source.cmake)


add_executable(ecs
    ecs.cpp
)


set_target_properties(ecs PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        CXX_SCAN_FOR_MODULES ON
        EXPERIMENTAL_CXX_MODULE_DYNDEP ON
)



target_sources(ecs
        PRIVATE
       ecs.cpp
)


target_compile_definitions(ecs PRIVATE
        NOMINMAX
        UNICODE
        _UNICODE
        ASIO_IMPL
)



target_link_libraries(ecs PRIVATE
        esc_lib_game
)

target_include_directories(ecs PRIVATE
        ${PLF_HIVE_INCLUDE_DIRS}
        ${SOL2_INCLUDE_DIRS}
        ${SQLITE_MODERN_CPP_INCLUDE_DIRS}
)

if(BUILD_TESTING)
add_subdirectory(test)
endif ()
