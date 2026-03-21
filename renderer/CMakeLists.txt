file(GLOB_RECURSE RENDERER_SOURCE
        LIST_DIRECTORIES false
        RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/*.ixx
)


add_library(renderer STATIC)

target_sources_by_extension(renderer ${RENDERER_SOURCE})

set_target_properties(renderer PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        CXX_SCAN_FOR_MODULES ON
        EXPERIMENTAL_CXX_MODULE_DYNDEP ON
)


target_link_libraries(renderer
        PUBLIC
        ecs
)
