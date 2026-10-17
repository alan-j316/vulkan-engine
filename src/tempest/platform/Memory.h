#pragma once
/**
 * @file Memory.h
 * @author Alan Abraham P Kochumon
 * @date Created on: October 17, 2026
 *
 * @brief Platform agnostic memory allocation management.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include <cstdint>

namespace tempest::platform
{
    /// Allocate a memory block of size @p size.
    void* alloc(size_t size, size_t alignment = sizeof(void*));

    /// Free an allocated @p block of memory.
    void free(void* block, size_t size);

    /// Reallocate a block of memory.
    void* realloc(void* block, size_t size);

    /// Zero-out a  block of memory.
    void* zeroOut(void* block, size_t size);

    /// Copy contents of memory from @p src to @p dst.
    void* copyMem(void* dst, void* src, size_t size);

    /// Write @p val to the entire @p dst block.
    void* setMem(void* dst, uint32_t val, size_t size);
} // namespace tempest::platform