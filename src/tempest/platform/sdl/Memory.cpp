/**
 * @file Memory.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: October 17, 2026
 *
 * @brief Generic/SDL implementation of Memory functions.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "platform/Memory.h"

#include <cstdlib>
#include <cstring>

namespace tempest::platform
{
    void* alloc(const size_t size, [[maybe_unused]] const size_t alignment)
    {
        // TODO: Use custom allocator or aligned alloc
        return std::malloc(size);
    }

    void free(void* block, [[maybe_unused]] const size_t size) { return std::free(block); }

    void* realloc(void* block, const size_t size) { return std::realloc(block, size); }

    void* zeroOut(void* block, const size_t size) { return std::memset(block, 0, size); }

    void* copyMem(void* dst, const void* src, const size_t size) { return std::memcpy(dst, src, size); }

    void* setMem(void* dst, const uint32_t val, const size_t size) { return std::memset(dst, val, size); }

} // namespace tempest::platform
