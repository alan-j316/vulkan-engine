#pragma once
/**
 * @file PMRResource.h
 * @author Alan Abraham P Kochumon
 * @date Created on: October 21, 2026
 *
 * @brief Thin wrapper around custom allocator for C++ PMR.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "MemorySubsystem.h"

#include <memory_resource>

namespace tempest::container
{
    class TempestPMR: public std::pmr::memory_resource
    {
    public:
        ~TempestPMR() noexcept override;

    private:
        void* do_allocate(const size_t size, const size_t alignment) override
        { return memory::alloc(size, memory::MemoryTag::MEMORY_TAG_PMR, alignment); }

        // TODO: Add alignment support
        void do_deallocate(void* ptr, const size_t size, [[maybe_unused]] size_t alignment) override
        {
            memory::free(ptr, size, memory::MemoryTag::MEMORY_TAG_PMR);
        }

        [[nodiscard]] bool do_is_equal(const memory_resource& other) const noexcept override
        {
            return *this == other;
        }
    };
} // namespace tempest::container