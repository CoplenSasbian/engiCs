

include(source.cmake)

add_executable(ecs)

set_target_properties(ecs PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        CXX_SCAN_FOR_MODULES ON
        EXPERIMENTAL_CXX_MODULE_DYNDEP ON
)


target_sources(ecs
        PRIVATE
        ${ECS_SOURCE}
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        FILES
        ${ECS_MODULE}
)


target_compile_definitions(ecs PRIVATE
        NOMINMAX
        UNICODE
        _UNICODE
)

find_package(mimalloc CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(lua CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS container)
find_package(libuv CONFIG REQUIRED)
#find_package(STDEXEC REQUIRED)

find_path(SQLITE_MODERN_CPP_INCLUDE_DIRS "sqlite_modern_cpp.h")
find_path(PLF_HIVE_INCLUDE_DIRS "plf_hive.h")
find_path(SOL2_INCLUDE_DIRS "sol/sol.hpp")

target_include_directories(ecs PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${SQLITE_MODERN_CPP_INCLUDE_DIRS}
        ${PLF_HIVE_INCLUDE_DIRS}
        ${SOL2_INCLUDE_DIRS}
        ${LUA_INCLUDE_DIR}
)
target_link_libraries(ecs PRIVATE
        STDEXEC::stdexec
        mimalloc
        unofficial::sqlite3::sqlite3
        spdlog::spdlog
        Vulkan::Vulkan
        lua
        Boost::container
        libuv::uv
)

