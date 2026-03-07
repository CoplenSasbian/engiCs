
add_executable( app
        ecs.cpp
)



file(GLOB_RECURSE APP_SOURCE
        LIST_DIRECTORIES false
        RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/*.ixx
)

target_sources_by_extension(app ${APP_SOURCE})

set_target_properties(app PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        CXX_SCAN_FOR_MODULES ON
        EXPERIMENTAL_CXX_MODULE_DYNDEP ON
)
find_package(mimalloc CONFIG REQUIRED)

target_link_libraries(app
        mimalloc
        ecs

)

