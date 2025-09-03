set(SOURCE_DIR "Source")

# define headers
file(GLOB_RECURSE HEADER_FILES "${SOURCE_DIR}/**.h")

# define source
file(GLOB_RECURSE SOURCE_FILES "${SOURCE_DIR}/**.cpp")

# define resources
set(RESOURCE_DIR "Resources")

# add the executable target
add_executable(VEngine ${HEADER_FILES} ${SOURCE_FILES})
add_compile_definitions(GLFW_INCLUDE_VULKAN)

target_link_libraries(VEngine glfw)
target_link_libraries(VEngine glm)
target_link_libraries(VEngine Vulkan::Vulkan)

target_include_directories(VEngine PRIVATE 
    "${SOURCE_DIR}/Platform"
    "${SOURCE_DIR}/Engine")

# compile shaders
find_program(GLSL_VALIDATOR glslangValidator HINTS /usr/bin /usr/local/bin $ENV{VULKAN_SDK}/Bin/ $ENV{VULKAN_SDK}/Bin32/)
file(GLOB_RECURSE GLSL_SOURCE_FILES
    "${RESOURCE_DIR}/Shaders/**.frag"
    "${RESOURCE_DIR}/Shaders/**.vert"
    "${RESOURCE_DIR}/Shaders/**.comp"
    )

foreach(GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${PROJECT_BINARY_DIR}/Resources/Shaders/${FILE_NAME}.spv")
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMENT "Compiling SPIR-V ${FILE_NAME}"
        COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
        DEPENDS ${GLSL}
        VERBATIM)
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(Shaders DEPENDS ${SPIRV_BINARY_FILES})
add_dependencies(${PROJECT_NAME} Shaders)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:VEngine>/Resources/Shaders/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${PROJECT_BINARY_DIR}/Resources/Shaders"
    "$<TARGET_FILE_DIR:VEngine>/Resources/Shaders"
)

# define resources in binaries
file(COPY ${RESOURCE_DIR} DESTINATION "${CMAKE_BINARY_DIR}/Resources/")