


find_package(mimalloc CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(lua CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS  container asio  locale)

find_path(SQLITE_MODERN_CPP_INCLUDE_DIRS "sqlite_modern_cpp.h")
find_path(PLF_HIVE_INCLUDE_DIRS "plf_hive.h")
find_path(SOL2_INCLUDE_DIRS "sol/sol.hpp")


file(GLOB_RECURSE ECS_SOURCE
        LIST_DIRECTORIES false
        RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/*.ixx
)




add_library(ecs STATIC)

target_sources_by_extension(ecs ${ECS_SOURCE})


set_target_properties(ecs PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        CXX_SCAN_FOR_MODULES ON
        EXPERIMENTAL_CXX_MODULE_DYNDEP ON
)




target_compile_definitions(ecs PRIVATE
        NOMINMAX
        UNICODE
        _UNICODE
        ASIO_IMPL
)



target_link_libraries(ecs PRIVATE
        mimalloc
        Boost::asio
        Boost::locale
        Boost::container
        unofficial::sqlite3::sqlite3
        spdlog::spdlog
        Vulkan::Vulkan
        PUBLIC
        STDEXEC::stdexec

)

target_include_directories(ecs PRIVATE
        ${PLF_HIVE_INCLUDE_DIRS}
        ${SOL2_INCLUDE_DIRS}
        ${SQLITE_MODERN_CPP_INCLUDE_DIRS}
)

