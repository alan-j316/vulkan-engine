set(TEMPEST_RENDERER_VULKAN_LIB TempestRendererVulkan${TEMPEST_INTERNAL_TARGET_SUFFIX})
add_library(
        ${TEMPEST_RENDERER_VULKAN_LIB}
        OBJECT
        VulkanBackend.cpp
        VulkanDevice.cpp
        VulkanSwapChain.cpp
        VulkanImage.cpp
        VulkanCommandPool.cpp
        VulkanFence.cpp
)


target_sources(${TEMPEST_RENDERER_VULKAN_LIB} PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${TEMPEST_ROOT_DIR}/renderer/vulkan
        FILES
        VulkanBackend.h

        PRIVATE
        VulkanTypes.inl
        VulkanDevice.h
        VulkanPlatform.h
        VulkanSwapChain.h
        VulkanImage.h
        VulkanCommandPool.h
        VulkanFence.h
)

add_library(Tempest::Internal::Renderer::Vulkan ALIAS ${TEMPEST_RENDERER_VULKAN_LIB})

target_compile_features(${TEMPEST_RENDERER_VULKAN_LIB} PRIVATE cxx_std_23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

target_include_directories(${TEMPEST_RENDERER_VULKAN_LIB} PUBLIC ${TEMPEST_ROOT_DIR})

target_link_libraries(
        ${TEMPEST_RENDERER_VULKAN_LIB} PRIVATE
        SDL3::SDL3-static
        Vulkan::Headers
        Vulkan::Hpp
)