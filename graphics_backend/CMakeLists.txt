add_library(GraphicsBackend
        graphics_backend.h
        graphics_backend_debug.cpp
        graphics_backend_debug.h
        graphics_backend_api.cpp
        graphics_backend_api.h
        enums/texture_type.h
        enums/texture_parameter.h
        enums/texture_target.h
        enums/texture_level_parameter.h
        enums/texture_internal_format.h
        enums/texture_pixel_format.h
        enums/texture_data_type.h
        enums/texture_unit.h
        enums/framebuffer_target.h
        enums/framebuffer_attachment.h
        enums/sampler_parameter.h
        enums/texture_wrap_mode.h
        enums/texture_filtering_mode.h
        enums/primitive_type.h
        enums/buffer_bind_target.h
        enums/vertex_attribute_data_type.h
        enums/buffer_usage_hint.h
        enums/graphics_backend_capability.h
        enums/blend_factor.h
        enums/cull_face.h
        enums/depth_function.h
        enums/shader_type.h
        enums/shader_parameter.h
        enums/program_parameter.h
        enums/uniform_data_type.h
        enums/clear_mask.h
        enums/uniform_block_parameter.h
        enums/uniform_parameter.h
        enums/cull_face_orientation.h
        enums/indices_data_type.h
        types/graphics_backend_texture.h
        types/graphics_backend_sampler.h
        types/graphics_backend_vao.h
        types/graphics_backend_buffer.h
        types/graphics_backend_program.h
        types/graphics_backend_shader_object.h
        types/graphics_backend_uniform_location.h
        types/graphics_backend_framebuffer.h
        enums/program_interface.h
        enums/program_interface_parameter.h
        enums/program_resource_parameter.h)

# windows
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")

    set(MINOR_VERSION 6)

    # glew for windows
    find_package(GLEW REQUIRED)
    set(INCLUDE_DIRS ${GLEW_INCLUDE_DIRS})
    target_link_libraries(GraphicsBackend ${GLEW_LIBRARIES} glu32.lib opengl32.lib)

# macos
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

    set(MINOR_VERSION 1)

    # opengl for macos
    find_package(OpenGL REQUIRED)
    set(INCLUDE_DIRS ${OPENGL_INCLUDE_DIRS})
    target_link_libraries(GraphicsBackend ${OPENGL_LIBRARIES})

endif()

include_directories(${INCLUDE_DIRS})

target_compile_definitions(GraphicsBackend PUBLIC
        GL_SILENCE_DEPRECATION
        ${PLATFORM_DEFINE}
        GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
        OPENGL_MAJOR_VERSION=4
        OPENGL_MINOR_VERSION=${MINOR_VERSION})

target_include_directories(GraphicsBackend PUBLIC . ${INCLUDE_DIRS})
target_link_libraries(GraphicsBackend DebugUtil)