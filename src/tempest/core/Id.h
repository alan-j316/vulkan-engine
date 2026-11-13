#pragma once
/**
 * @file Id.h
 * @author Alan Abraham P Kochumon
 * @date Created on: November 13, 2026
 *
 * @brief Unique Identifiers.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include <cstdint>
#include <limits>

namespace tempest
{
    using UID = uint32_t;
    inline constexpr UID INVALID_UID = std::numeric_limits<UID>::max();

} // namespace tempest