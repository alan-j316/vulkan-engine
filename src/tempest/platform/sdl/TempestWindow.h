#pragma once
/**
 * @file TempestWindow.h
 * @author Alan Abraham P Kochumon
 * @date Created on: September 23, 2026
 *
 * @brief Manages application windowing.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include <string>

namespace tempest::platform
{
    struct WindowExtent
    {
        size_t width;
        size_t height;
    };

    class TempestWindow
    {
    public:
        TempestWindow(size_t width, size_t height, std::string windowName);
        bool initWindow() noexcept;

        ~TempestWindow() noexcept;

        TempestWindow(const TempestWindow& other)            = delete;
        TempestWindow& operator=(const TempestWindow& other) = delete;
        TempestWindow(TempestWindow&& other) noexcept;
        TempestWindow& operator=(TempestWindow&& other) noexcept;

        [[nodiscard]] void* getBaseWindow() const noexcept;
        [[nodiscard]] constexpr size_t getWidth() const noexcept { return _width; }
        [[nodiscard]] constexpr size_t getHeight() const noexcept { return _height; }

        static char const* const* getRequiredExtensions(uint32_t& count) noexcept;

        WindowExtent getWindowExtent() const noexcept;

    private:
        void* _window;
        size_t _width, _height;
    };
} // namespace tempest::platform