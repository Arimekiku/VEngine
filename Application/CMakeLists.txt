set(SOURCE_DIR "Source")

# define headers
file(GLOB_RECURSE HEADER_FILES "${SOURCE_DIR}/**.h")

# define source
file(GLOB_RECURSE SOURCE_FILES "${SOURCE_DIR}/**.cpp")

# Add the executable target
add_executable(VEngine ${HEADER_FILES} ${SOURCE_FILES})

target_link_libraries(VEngine glfw)
target_link_libraries(VEngine glm)
target_link_libraries(VEngine Vulkan::Vulkan)

target_include_directories(VEngine PRIVATE "${SOURCE_DIR}/Platform")