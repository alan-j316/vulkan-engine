#pragma once
/**
 * @file VulkanImage.h
 * @author Alan Abraham P Kochumon
 * @date Created on: October 22, 2026
 *
 * @brief Abstraction for Rendering Image and ImageView.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "renderer/vulkan/VulkanTypes.inl"


namespace tempest::renderer
{
    class VulkanImage
    {
    public:
        VulkanImage() noexcept = default;

        bool create(const VulkanContext& context, const VulkanImageCreateInfo& createInfo) noexcept;
        bool destroy() noexcept;

        [[nodiscard]] const vk::raii::Image& getBaseImage() const noexcept { return _image; }
        [[nodiscard]] const vk::raii::ImageView& getBaseImageView() const noexcept { return _imageView; }
        // [[nodiscard]] const vk::ImageLayout& getImageLayout() const noexcept { return _layout; }

        // void transitionImageLayout(vk::ImageLayout newLayout, const CommandBuffer& commandBuffer,
        //                            size_t commandBufferIndex = 0) const noexcept;
        //
        // /// Copy data from a staging buffer to this image.
        // void copyFromBuffer(const Buffer& buffer, const CommandBuffer& commandBuffer,
        //                     size_t commandBufferIndex = 1) const noexcept;
        //
        // void generateMipmaps(const RenderDevice& device, const CommandBuffer& commandBuffer,
        //                      size_t commandBufferIndex = 0) const noexcept;

    private:
        bool createImageView(const VulkanContext& context, vk::Format format,
                             vk::ImageAspectFlags aspectFlags) noexcept;
        // bool createImage(const RenderDevice& device) noexcept;

    private:
        vk::raii::Image _image{ nullptr };
        vk::raii::ImageView _imageView{ nullptr };
        vk::raii::DeviceMemory _memory{ nullptr };
        uint32_t _mipLevels{ 0 };
        uint32_t _width{ 0 }, _height{ 0 };
        // vk::ImageLayout _layout{ vk::ImageLayout::eUndefined };
        // vk::Format _format{};
        // vk::ImageTiling _tiling{};
        // vk::ImageUsageFlags _usageFlags{};
        // vk::MemoryPropertyFlags _memoryFlags{};
        // vk::SampleCountFlagBits _samples{};
        // vk::ImageAspectFlags _aspectFlags{};
    };

} // namespace tempest::renderer