add_library(GraphicsBackend
        graphics_backend.h
        graphics_backend_debug.cpp
        graphics_backend_debug.h
        graphics_backend_api.cpp
        graphics_backend_api.h
        enums/texture_type.h
        enums/texture_internal_format.h
        enums/framebuffer_target.h
        enums/framebuffer_attachment.h
        enums/texture_wrap_mode.h
        enums/texture_filtering_mode.h
        enums/primitive_type.h
        enums/buffer_bind_target.h
        enums/vertex_attribute_data_type.h
        enums/buffer_usage_hint.h
        enums/blend_factor.h
        enums/cull_face.h
        enums/depth_function.h
        enums/shader_type.h
        enums/uniform_data_type.h
        enums/cull_face_orientation.h
        enums/indices_data_type.h
        types/graphics_backend_texture.h
        types/graphics_backend_sampler.h
        types/graphics_backend_geometry.h
        types/graphics_backend_buffer.h
        types/graphics_backend_program.h
        types/graphics_backend_shader_object.h
        types/graphics_backend_framebuffer.h
        enums/program_resource_parameter.h
        enums/blit_framebuffer_mask.h
        enums/blit_framebuffer_filter.h
        enums/graphics_backend_name.h
        graphics_backend_implementations/graphics_backend_api_base.cpp
        graphics_backend_implementations/graphics_backend_api_base.h
        graphics_backend_implementations/graphics_backend_api_opengl.cpp
        graphics_backend_implementations/graphics_backend_api_opengl.h
        graphics_backend_implementations/graphics_backend_api_metal.cpp
        graphics_backend_implementations/graphics_backend_api_metal.h
        types/graphics_backend_vertex_attribute_descriptor.h
        types/graphics_backend_resource_bindings.h
        helpers/opengl_helpers.h
        helpers/opengl_helpers.cpp
        helpers/metal_helpers.h
        helpers/metal_helpers.cpp
        enums/load_action.h
        enums/store_action.h
        types/graphics_backend_render_target_descriptor.h
        types/graphics_backend_depth_stencil_state.h
        types/graphics_backend_color_attachment_descriptor.h)

if (${RENDER_ENGINE_WINDOWS_PLATFORM})

    set(MINOR_VERSION 6)

    # glew for windows
    find_package(GLEW REQUIRED)
    set(INCLUDE_DIRS ${GLEW_INCLUDE_DIRS})
    target_link_libraries(GraphicsBackend ${GLEW_LIBRARIES} glu32.lib opengl32.lib)

elseif (${RENDER_ENGINE_APPLE_PLATFORM})

    set(MINOR_VERSION 1)

    # opengl for macos
    find_package(OpenGL REQUIRED)
    set(INCLUDE_DIRS ${OPENGL_INCLUDE_DIRS})
    target_link_libraries(GraphicsBackend ${OPENGL_LIBRARIES})

    # metal for macos
    target_link_libraries(GraphicsBackend metal-cpp)

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