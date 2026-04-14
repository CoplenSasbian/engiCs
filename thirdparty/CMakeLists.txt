

set(BUILD_TESTING OFF)
set(STDEXEC_BUILD_EXAMPLES OFF)
add_subdirectory("stdexec")

set(SPIRV_REFLECT_EXECUTABLE OFF)
set(SPIRV_REFLECT_STATIC_LIB ON)
add_subdirectory("SPIRV-Reflect")
