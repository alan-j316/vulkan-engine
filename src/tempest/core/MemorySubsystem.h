#pragma once
/**
 * @file TempestMemory.h
 * @author Alan Abraham P Kochumon
 * @date Created on: October 17, 2026
 *
 * @brief Memory subsystem for Tempest.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "Api.h"

#include <cstdint>

namespace tempest
{
    enum class MemoryTag : uint8_t
    {
        MEMORY_TAG_UNKNOWN,
        MEMORY_TAG_ARRAY,
        MEMORY_TAG_VECTOR,
        MEMORY_TAG_DICT,
        MEMORY_TAG_RINGS_QUEUE,
        MEMORY_TAG_STRING,
        MEMORY_TAG_BST,
        MEMORY_TAG_APPLICATION,
        MEMORY_TAG_JOB,
        MEMORY_TAG_TEXTURE,
        MEMORY_TAG_MATERIAL_INSTANCE,
        MEMORY_TAG_RENDERER,
        MEMORY_TAG_GAME,
        MEMORY_TAG_TRANSFORM,
        MEMORY_TAG_ENTITY,
        MEMORY_TAG_ENTITY_NODE,
        MEMORY_TAG_SCENE,


        MEMORY_TAG_MAX_TAGS
    };


    struct MemorySubsystem
    {
        TEMPEST_API void initialize();

        TEMPEST_API void shutdown();

        TEMPEST_API void* alloc(size_t size, MemoryTag tag);

        TEMPEST_API void free(void* block, size_t size, MemoryTag tag);

        TEMPEST_API void* zeroOut(void* block, size_t size);

        TEMPEST_API void* memCopy(void* dst, void* src, size_t size);

        TEMPEST_API void* memSet(void* dst, uint32_t value, size_t size);
    };
} // namespace tempest
