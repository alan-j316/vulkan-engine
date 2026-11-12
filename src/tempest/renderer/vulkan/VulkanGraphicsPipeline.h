#pragma once
/**
 * @file VulkanGraphicsPipeline.h
 * @author Alan Abraham P Kochumon
 * @date Created on: October 29, 2026
 *
 * @brief Vulkan Graphics Pipeline.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "Application.h"
#include "VulkanSwapChain.h"
#include "containers/Vector.h"
#include "renderer/RendererTypes.inl"
#include "renderer/vulkan/VulkanBuffer.h"
#include "renderer/vulkan/VulkanDevice.h"
#include "renderer/vulkan/VulkanTypes.inl"

#include <cstdint>


namespace tempest::renderer
{
    struct GraphicsPipelineConfig
    {
        std::string vertShaderEntryPoint;
        std::vector<char> pVertShaderCode; // TODO: Change to PMR
        std::string fragShaderEntryPoint;
        std::vector<char> pFragShaderCode; // TODO: Change to PMR

        bool wireframe{ false };            /// Indicate whether pipeline draws only wireframes
        vk::Bool32 enableMsaa{ vk::False }; ///< Enable/Disable MSAA
        vk::SampleCountFlagBits msaaSamples{
            vk::SampleCountFlagBits::e1
        }; ///< MSAA sample count. Ignored if msaa is disabled
        vk::FrontFace winding{ vk::FrontFace::eClockwise };                     ///< Triangle winding order
        vk::CullModeFlags cullMode{ vk::CullModeFlagBits::eNone };              ///< Face culling
        vk::PrimitiveTopology topology{ vk::PrimitiveTopology::eTriangleList }; ///< Primitive topology
    };

    // TODO: Update to PMR vector

    using ShaderIndex = uint8_t;

    class VulkanGraphicsPipeline
    {
    public:
        /**
         * Create a graphics pipeline. Must only be called once per instance.
         * @param device The vulkan device.
         * @param swapChain
         * @param config The graphics pipeline configuration.
         * @param vmaAllocator
         * @return `true` if pipeline was create successfully and `false` otherwise.
         */
        bool create(const VulkanDevice& device, const VulkanSwapChain& swapChain, const GraphicsPipelineConfig& config,
                    const VmaAllocator& vmaAllocator);

        /**
         * Create a shader. The shader is not bound to the pipeline and must be manually bound using @ref bindShader.
         * @param device The vulkan device.
         * @param pShaderCode The shader code binary.
         * @return An internal handle to the shader.
         */
        ShaderIndex createShader(const VulkanDevice& device, const std::vector<char>& pShaderCode);

        /**
         * Updates the global uniform buffer object for the frame.
         * @param frameIndex The index of the current rendering frame.
         * @param ubo        The uniform object to update with.
         */
        void updateGlobalUBO(size_t frameIndex, const GlobalUniform& ubo) const noexcept;


        /**
         * Destroy the render pipeline.
         */
        void destroy();

        /// Get the vulkan graphics pipeline.
        [[nodiscard]] const vk::raii::Pipeline& getBasePipeline() const { return _pipeline; }

        /// Get the vulkan graphics pipeline.
        [[nodiscard]] vk::raii::Pipeline& getBasePipeline() { return _pipeline; }

        /// Get the global descriptor set for the current @p frameIndex.
        [[nodiscard]] vk::DescriptorSet descriptorSet(const uint32_t frameIndex) const noexcept
        { return *_globalDescriptorSets[frameIndex]; }

        /// Get the vulkan pipeline layout.
        [[nodiscard]] vk::PipelineLayout getLayout() const noexcept { return *_pipelineLayout; }


    private:
        vk::raii::PipelineLayout _pipelineLayout{ nullptr };
        vk::raii::Pipeline _pipeline{ nullptr };
        Vector<vk::raii::ShaderModule> _shaders{};
        // TODO: Temp; requires refactor.

        // Global Uniform
        vk::raii::DescriptorPool _globalDescriptorPool{ nullptr };
        // TODO: Update from std::vector to stack-allocated array
        std::vector<vk::raii::DescriptorSet> _globalDescriptorSets{}; // 3 for triple buffering.
        vk::raii::DescriptorSetLayout _globalDescriptorSetLayout{ nullptr };
        Vector<VulkanBuffer> _globalUniformBuffers{};
        Vector<void*> _uniformBufferPtr{ makeVector<void*>() }; // Used for persistent mapping
    };
} // namespace tempest::renderer