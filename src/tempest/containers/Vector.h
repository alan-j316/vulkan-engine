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

#include <vector>

namespace tempest
{
    template <typename T>
    using Vector = std::pmr::vector<T>;
}