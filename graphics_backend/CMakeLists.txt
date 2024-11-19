add_library(GraphicsBackend
        graphics_backend_debug_group.cpp
        graphics_backend_debug_group.h
        graphics_backend_api.cpp
        graphics_backend_api.h
        enums/texture_type.h
        enums/texture_internal_format.h
        enums/framebuffer_attachment.h
        enums/texture_wrap_mode.h
        enums/texture_filtering_mode.h
        enums/primitive_type.h
        enums/vertex_attribute_data_type.h
        enums/blend_factor.h
        enums/cull_face.h
        enums/depth_function.h
        enums/shader_type.h
        enums/texture_data_type.h
        enums/cull_face_orientation.h
        enums/indices_data_type.h
        types/graphics_backend_texture.h
        types/graphics_backend_sampler.h
        types/graphics_backend_geometry.h
        types/graphics_backend_buffer.h
        types/graphics_backend_program.h
        types/graphics_backend_shader_object.h
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
        types/graphics_backend_color_attachment_descriptor.h
        types/graphics_backend_texture_info.h
        types/graphics_backend_sampler_info.h
        enums/cubemap_face.h)

if (${RENDER_ENGINE_WINDOWS_PLATFORM})

    # glew for windows
    find_package(GLEW REQUIRED)
    target_include_directories(GraphicsBackend PUBLIC ${GLEW_INCLUDE_DIRS})
    target_link_libraries(GraphicsBackend ${GLEW_LIBRARIES} glu32.lib opengl32.lib)

    target_compile_definitions(GraphicsBackend PUBLIC RENDER_BACKEND_OPENGL OPENGL_MAJOR_VERSION=4 OPENGL_MINOR_VERSION=6)

elseif (${RENDER_ENGINE_APPLE_PLATFORM})

    # metal for macos
    target_link_libraries(GraphicsBackend metal-cpp)

    target_compile_definitions(GraphicsBackend PUBLIC RENDER_BACKEND_METAL)

endif()

target_include_directories(GraphicsBackend PUBLIC .)
target_link_libraries(GraphicsBackend DebugUtil)