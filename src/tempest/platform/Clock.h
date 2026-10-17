#pragma once
/**
 * @file Clock.h
 * @author Alan Abraham P Kochumon
 * @date Created on: October 17, 2026
 *
 * @brief Clock API interface.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

namespace tempest::platform
{
    double getAbsoluteTime();

    /// Sleep the thread for the given @p ms.
    /// @note This can block the main thread.
    void sleep(size_t ms);

} // namespace tempest::platform