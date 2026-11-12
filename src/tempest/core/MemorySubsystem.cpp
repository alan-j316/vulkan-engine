/**
 * @file MemorySubsystem.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: October 17, 2026
 *
 * @brief Implementation of member functions declared in TempestMemory.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "MemorySubsystem.h"

#include "Logger.h"
#include "platform/Platform.h"

namespace tempest::memory
{

    namespace
    {
        struct MemoryStats
        {
            size_t totalAllocations{};
            std::array<size_t, static_cast<size_t>(MemoryTag::MEMORY_TAG_MAX_TAGS)> taggedAllocation{};
        };
    } // namespace

    static MemoryStats stats;

    void init()
    {
        platform::zeroOut(stats.taggedAllocation.data(), stats.taggedAllocation.size());
        stats.totalAllocations = 0;
    }

    void shutdown()
    {
        // TODO: Impl
    }

    void* alloc(const size_t size, const MemoryTag tag, const size_t alignment)
    {
        if (tag == MemoryTag::MEMORY_TAG_UNKNOWN)
        {
            log::warn("Using unknown memory tag. Reclass the allocation");
        }

        stats.totalAllocations += size;
        stats.taggedAllocation[static_cast<size_t>(tag)] += size;

        void* block = platform::alloc(size, alignment);
        platform::zeroOut(block, size);


        // log::info(std::format("Allocating {} bytes on tag {}.", size, toString(tag)));
        // log::info(std::format("New State of allocator(IDX: {}): {}", static_cast<size_t>(tag),
        //                       stats.taggedAllocation[static_cast<size_t>(tag)]));
        // log::info(std::format("Total usage: {} bytes", stats.totalAllocations));

        return block;
    }

    void free(void* block, const size_t size, MemoryTag tag)
    {
        if (tag == MemoryTag::MEMORY_TAG_UNKNOWN)
        {
            log::warn("Using unknown memory tag. Reclass the allocation");
        }

        stats.totalAllocations -= size;
        stats.taggedAllocation[static_cast<size_t>(tag)] -= size;

        platform::free(block, size);
    }

    void* zeroOut(void* block, const size_t size) { return platform::zeroOut(block, size); }

    void* memCopy(void* dst, const void* src, const size_t size) { return platform::copyMem(dst, src, size); }

    void* memSet(void* dst, const uint32_t value, const size_t size) { return platform::setMem(dst, value, size); }


    std::string getMemUsageStr()
    {
        constexpr size_t GB = 1024 * 1024 * 1024;
        constexpr size_t MB = 1024 * 1024;
        constexpr size_t KB = 1024;

        std::string str =
            std::format("{:<20}:{:>10}\n", "Total usage", std::to_string(stats.totalAllocations) + std::string(" B"));
        for (size_t i = 0; i < static_cast<size_t>(MemoryTag::MEMORY_TAG_MAX_TAGS); ++i)
        {
            std::string byteValue;
            if (stats.taggedAllocation[i] > GB)
            {
                byteValue = std::format("{:.02} GB", (stats.taggedAllocation[i] / static_cast<float>(GB)));
            }
            else if (stats.taggedAllocation[i] > MB)
            {
                byteValue = std::format("{:.02} MB", (stats.taggedAllocation[i] / static_cast<float>(MB)));
            }
            else if (stats.taggedAllocation[i] > KB)
            {
                byteValue = std::format("{:.02} KB", (stats.taggedAllocation[i] / static_cast<float>(KB)));
            }
            else
            {
                byteValue = std::format("{} B", stats.taggedAllocation[i]);
            }
            const auto formatted = std::format("{:<20}:{:>10}\n", toString(static_cast<MemoryTag>(i)), byteValue);
            str += formatted;
        }
        return str;
    }



} // namespace tempest::memory