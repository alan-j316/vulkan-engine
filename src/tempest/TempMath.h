#pragma once
/**
 * @file TempMath.h
 * @author Alan Abraham P Kochumon
 * @date Created on: November 12, 2026
 *
 * @brief TEMPORARY ENGINE: TO REMOVE WHEN FALCON SUPPORTS THESE
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

// TODO: Temporary REMOVE
// NOTE: fov must be in radians
namespace tmp
{
    static flcn::Mat4F makePersp(const float aspect, const float fov, const float far, const float near)
    {
        const float s1 = 1 / (std::tan(fov * 0.5f) * aspect);
        const float s2 = 1 / std::tan(fov * 0.5f);
        return flcn::Mat4F{ s1, 0, 0,  0, 0, -s2, 0, 0, 0, 0, -far / (far - near), -(far * near) / (far - near),
                            0,  0, -1, 0 };
    }
} // namespace tmp