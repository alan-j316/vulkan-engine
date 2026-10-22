set(TEMPEST_RENDERER_LIB TempestRenderer${TEMPEST_INTERNAL_TARGET_SUFFIX})
add_library(
        ${TEMPEST_RENDERER_LIB}
        OBJECT
        RendererFrontend.cpp
)

add_subdirectory(vulkan)

target_sources(${TEMPEST_RENDERER_LIB} PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${TEMPEST_ROOT_DIR}/renderer
        FILES
        RendererFrontend.h
        RendererBackend.h
        RendererTypes.inl
)

add_library(Tempest::Internal::Renderer ALIAS ${TEMPEST_RENDERER_LIB})

target_compile_features(${TEMPEST_RENDERER_LIB} PRIVATE cxx_std_23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

target_include_directories(${TEMPEST_RENDERER_LIB} PUBLIC ${TEMPEST_ROOT_DIR})

target_link_libraries(
        ${TEMPEST_RENDERER_LIB} PUBLIC # TODO: Update to private after init vulkan using pimpl idiom
        Tempest::Internal::Renderer::Vulkan
)