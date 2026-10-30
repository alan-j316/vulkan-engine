/**
 * @file VulkanGraphicsPipeline.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: October 29, 2026
 *
 * @brief Implementation of Vulkan graphics pipeline declared in VulkanGraphicsPipeline.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "VulkanGraphicsPipeline.h"

#include "VulkanSwapChain.h"

namespace tempest::renderer
{
    bool VulkanGraphicsPipeline::create(const VulkanDevice& device, const VulkanSwapChain& swapChain,
                                        const GraphicsPipelineConfig& config)
    {

        /// ----------------
        /// Shader creation
        /// ----------------
        const auto vertIdx = createShader(device, config.pVertShaderCode);
        const vk::PipelineShaderStageCreateInfo vertCreateInfo{ .stage  = vk::ShaderStageFlagBits::eVertex,
                                                                .module = _shaders[vertIdx],
                                                                .pName  = config.vertShaderEntryPoint.c_str() };

        const auto fragIdx = createShader(device, config.pFragShaderCode);
        const vk::PipelineShaderStageCreateInfo fragCreateInfo{ .stage  = vk::ShaderStageFlagBits::eFragment,
                                                                .module = _shaders[fragIdx],
                                                                .pName  = config.fragShaderEntryPoint.c_str() };
        vk::PipelineShaderStageCreateInfo stages[]{ vertCreateInfo, fragCreateInfo };


        /// ----------------
        /// Dynamic States
        /// ----------------
        Vector<vk::DynamicState> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
        vk::PipelineDynamicStateCreateInfo dynamicStateInfo{ .dynamicStateCount =
                                                                 static_cast<uint32_t>(dynamicStates.size()),
                                                             .pDynamicStates = dynamicStates.data() };

        /// ----------------
        /// Vertex Input
        /// ----------------
        vk::PipelineVertexInputStateCreateInfo vertexInfo{};


        /// ----------------
        /// Input Assembly
        /// ----------------
        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo{ .topology = config.topology };


        /// ----------------
        /// Viewport State
        /// ----------------
        // Viewport state is set dynamically when recording command buffer so we only need to specify the
        // viewport state.
        vk::PipelineViewportStateCreateInfo viewportStateInfo{ .viewportCount = 1, .scissorCount = 1 };


        /// ----------------
        /// Rasterizer
        /// ----------------
        vk::PipelineRasterizationStateCreateInfo rasterizationInfo{
            .depthClampEnable        = vk::False,
            .rasterizerDiscardEnable = vk::False,
            .polygonMode             = config.wireframe ? vk::PolygonMode::eLine : vk::PolygonMode::eFill,
            .cullMode                = config.cullMode,
            .frontFace               = config.winding,
            .depthBiasClamp          = vk::False,
            .lineWidth               = 1.0f,
        };


        /// ----------------
        /// MSAA
        /// ----------------
        vk::PipelineMultisampleStateCreateInfo multisamplingInfo{ .rasterizationSamples = config.msaaSamples,
                                                                  .sampleShadingEnable  = config.enableMsaa };


        /// ----------------
        /// Depth & Stencil
        /// ----------------
        // TODO:


        /// ----------------
        /// Color Blending
        /// ----------------
        /// TODO: Enable color blending
        vk::PipelineColorBlendAttachmentState colorBlendAttachmentState{
            .blendEnable    = vk::False,
            .colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
        };
        // vk::PipelineColorBlendAttachmentState colorBlendAttachment{
        //     .blendEnable         = vk::True,
        //     .srcColorBlendFactor = vk::BlendFactor::eSrcAlpha,
        //     .dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha,
        //     .colorBlendOp        = vk::BlendOp::eAdd,
        //     .srcAlphaBlendFactor = vk::BlendFactor::eOne,
        //     .dstAlphaBlendFactor = vk::BlendFactor::eZero,
        //     .alphaBlendOp        = vk::BlendOp::eAdd,
        //     .colorWriteMask      = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
        //     vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA};
        vk::PipelineColorBlendStateCreateInfo colorBlendingInfo{
            .logicOpEnable   = vk::False,
            .logicOp         = vk::LogicOp::eCopy,
            .attachmentCount = 1,
            .pAttachments    = &colorBlendAttachmentState,
        };

        /// ----------------
        /// Pipeline
        /// ----------------
        /// TODO: Make configurable
        vk::PipelineLayoutCreateInfo pipelineLayoutInfo{ .setLayoutCount = 0, .pushConstantRangeCount = 0 };
        _pipelineLayout = vk::raii::PipelineLayout{ device.getLogicalDevice(), pipelineLayoutInfo };

        const auto format = swapChain.getFormat().format;
        vk::PipelineRenderingCreateInfo pipelineRenderingInfo{ .colorAttachmentCount    = 1,
                                                               .pColorAttachmentFormats = &format };
        vk::GraphicsPipelineCreateInfo graphicsPipelineInfo{ .pNext               = &pipelineRenderingInfo,
                                                             .stageCount          = 2,
                                                             .pStages             = stages,
                                                             .pVertexInputState   = &vertexInfo,
                                                             .pInputAssemblyState = &inputAssemblyInfo,
                                                             .pViewportState      = &viewportStateInfo,
                                                             .pRasterizationState = &rasterizationInfo,
                                                             .pMultisampleState   = &multisamplingInfo,
                                                             .pDepthStencilState  = nullptr,
                                                             .pColorBlendState    = &colorBlendingInfo,
                                                             .pDynamicState       = &dynamicStateInfo,
                                                             .layout              = _pipelineLayout,
                                                             .renderPass          = nullptr };
        // vk::PipelineRenderingCreateInfo pipelineCreateInfo{
        //     .pNext                   = &graphicsPipelineInfo,
        //     .colorAttachmentCount    = 1,
        //     .pColorAttachmentFormats = &format,
        // };

        // TODO: Update with custom allocator
        _pipeline = vk::raii::Pipeline(device.getLogicalDevice(), nullptr, graphicsPipelineInfo, nullptr);

        return true;
    }

    ShaderIndex VulkanGraphicsPipeline::createShader(const VulkanDevice& device, const std::vector<char>& pShaderCode)
    {
        const vk::ShaderModuleCreateInfo shaderCreateInfo{ .codeSize = pShaderCode.size(),
                                                           .pCode =
                                                               reinterpret_cast<const uint32_t*>(pShaderCode.data()) };
        _shaders.emplace_back(device.getLogicalDevice(), shaderCreateInfo);
        return static_cast<ShaderIndex>(_shaders.size() - 1);
    }

    void VulkanGraphicsPipeline::bindShader(ShaderIndex index) {}

    void VulkanGraphicsPipeline::unbindShader(ShaderIndex index) {}

    void VulkanGraphicsPipeline::destroy()
    {
        _pipeline       = nullptr;
        _pipelineLayout = nullptr;
        _shaders.clear();
    }
} // namespace tempest::renderer
