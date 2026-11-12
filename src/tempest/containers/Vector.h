#pragma once
/**
 * @file Vector.h
 * @author Alan Abraham P Kochumon
 * @date Created on: October 21, 2026
 *
 * @brief PMR Vector
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "PMRResource.h"

#include <vector>
namespace tempest
{
    template <typename T>
    using Vector = std::pmr::vector<T>;

    // TODO: Move to application
    /// Get a @ref Vector instance with the custom PMR allocator.
    template <typename T>
    Vector<T> makeVector()
    {
        static container::TempestPMR pmrAllocator;
        return Vector<T>(&pmrAllocator);
    }
} // namespace tempest