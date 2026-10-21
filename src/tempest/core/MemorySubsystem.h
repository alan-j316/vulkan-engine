#pragma once
/**
 * @file MemorySubsystem.h
 * @author Alan Abraham P Kochumon
 * @date Created on: October 17, 2026
 *
 * @brief Memory subsystem for Tempest.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "Api.h"

#include <array>
#include <cstdint>
#include <string>

namespace tempest::memory
{
    enum class TEMPEST_API MemoryTag : uint8_t
    {
        MEMORY_TAG_UNKNOWN,
        MEMORY_TAG_ARRAY,
        MEMORY_TAG_PMR,
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

    constexpr TEMPEST_API std::string_view toString(const MemoryTag tag)
    {
        switch (tag)
        {
            case MemoryTag::MEMORY_TAG_UNKNOWN:
                return "Unknown";
            case MemoryTag::MEMORY_TAG_ARRAY:
                return "Array";
            case MemoryTag::MEMORY_TAG_PMR:
                return "C++ PMR";
            case MemoryTag::MEMORY_TAG_DICT:
                return "Dict";
            case MemoryTag::MEMORY_TAG_RINGS_QUEUE:
                return "Ring Queue";
            case MemoryTag::MEMORY_TAG_STRING:
                return "String";
            case MemoryTag::MEMORY_TAG_BST:
                return "BST";
            case MemoryTag::MEMORY_TAG_APPLICATION:
                return "Application";
            case MemoryTag::MEMORY_TAG_JOB:
                return "Job System";
            case MemoryTag::MEMORY_TAG_TEXTURE:
                return "Texture";
            case MemoryTag::MEMORY_TAG_MATERIAL_INSTANCE:
                return "Material Instance";
            case MemoryTag::MEMORY_TAG_RENDERER:
                return "Renderer";
            case MemoryTag::MEMORY_TAG_GAME:
                return "Game";
            case MemoryTag::MEMORY_TAG_TRANSFORM:
                return "Transform";
            case MemoryTag::MEMORY_TAG_ENTITY:
                return "Entity";
            case MemoryTag::MEMORY_TAG_ENTITY_NODE:
                return "Entity Node";
            case MemoryTag::MEMORY_TAG_SCENE:
                return "Scene";
            case MemoryTag::MEMORY_TAG_MAX_TAGS:
                return "Max Tags(Unused)";
        }
        return "Error: Recheck Tag";
    }


    TEMPEST_API void init();

    TEMPEST_API void shutdown();

    TEMPEST_API void* alloc(size_t size, MemoryTag tag, size_t alignment = sizeof(void*));

    TEMPEST_API void free(void* block, size_t size, MemoryTag tag);

    TEMPEST_API void* zeroOut(void* block, size_t size);

    TEMPEST_API void* memCopy(void* dst, const void* src, size_t size);

    TEMPEST_API void* memSet(void* dst, uint32_t value, size_t size);

    TEMPEST_API std::string getMemUsageStr();
    ;
} // namespace tempest::memory
