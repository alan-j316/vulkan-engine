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

#include "containers/Vector.h"
#include "renderer/vulkan/VulkanDevice.h"
#include "renderer/vulkan/VulkanTypes.inl"

#include <cstdint>


namespace tempest::renderer
{
    struct GraphicsPipelineConfig
    {
        std::string vertShaderEntryPoint;
        std::vector<char>& pVertShaderCode; // TODO: Change to PMR
        std::string fragShaderEntryPoint;
        std::vector<char>& pFragShaderCode; // TODO: Change to PMR

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
         * @param config The graphics pipeline configuration.
         * @return `true` if pipeline was create successfully and `false` otherwise.
         */
        bool create(const VulkanDevice& device, const GraphicsPipelineConfig& config);

        /**
         * Create a shader. The shader is not bound to the pipeline and must be manually bound using @ref bindShader.
         * @param device The vulkan device.
         * @param pShaderCode The shader code binary.
         * @return An internal handle to the shader.
         */
        ShaderIndex createShader(const VulkanDevice& device, const std::vector<char>& pShaderCode);

        /**
         * Bind a shader the pipeline.
         * @param index The shader index to the shader to bind.
         */
        void bindShader(ShaderIndex index);

        /**
         * Unbind a shader from the pipeline.
         * @param index The index of the shader to unbind.
         */
        void unbindShader(ShaderIndex index);


        /**
         * Destroy the render pipeline.
         */
        void destroy();

    private:
        vk::raii::PipelineLayout _pipelineLayout{ nullptr };
        vk::raii::Pipeline _pipeline{ nullptr };
        Vector<vk::raii::ShaderModule> _shaders{};
    };
} // namespace tempest::renderer