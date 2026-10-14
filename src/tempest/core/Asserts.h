#pragma once
/**
 * @file Asserts.h
 * @author Alan Abraham P Kochumon
 * @date Created on: October 14, 2026
 *
 * @brief Assertions library for tempest.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include <iostream> // TODO: Replace with logger



/**
 * @brief Triggers a compiler-specific breakpoint.
 */
#ifdef _MSC_VER
    #define TEMPEST_DEBUG_BREAK() __debugbreak()
#elif defined(__clang__) || defined(__GNUC__)
    #define TEMPEST_DEBUG_BREAK() __builtin_trap()
#else
    #include <signal.h>
    #define TEMPEST_DEBUG_BREAK() raise(SIGTRAP)
#endif

/**
 * @brief API directive for handle DLL exports.
 */
#ifdef TEMPEST_DLL
    #ifdef _WIN32
        #ifdef TEMPEST_LIB_BUILD
            #define TEMPEST_API __declspec(dllexport)
        #else
            #define TEMPEST_API __declspec(dllimport)
        #endif
    #else
        #if __GNUC__ >= 4 || defined(__clang__)
            #define TEMPEST_API __attribute__((visibility("default")))
        #else
            #define TEMPEST_API
        #endif
    #endif
#else
    #define TEMPEST_API
#endif



namespace tempest::internal
{
    /**
     * @brief An internal function used for triggering compile time error with assertions.
     *        Since all the functions are marked as noexcept, throwing inside `std::is_constant_evaluated`
     *        will not work. So by calling a non constexpr function inside a constexpr function, it will trigger
     *        a no-compile error.
     */
    inline void compileTimeErrorTrap() {}

    /**
     * @brief Log the assertion message and status to console.
     *
     * @param condition The condition that triggered the assertion.
     * @param message   An optional message, stating the reason behind assertion failure.
     * @param file      The name of the file that triggered the assertion.
     * @param line      The line number of hte code that triggered the assertion.
     */
    inline void logAssertion(const char* condition, const char* message, const char* file, const int line)
    {
        std::cerr << "[TEMPEST ASSERTION FAILED]\n"
                  << "Condition: " << condition << '\n'
                  << "Message:   " << (message ? message : "None") << '\n'
                  << "Location:  " << file << ":" << line << '\n';
    }

} // namespace tempest::internal



/**
 * @def TEMPEST_ASSERT(condition, message)
 * @brief Asserts a condition is `true`, breaking into the debugger with a custom message if `false`.
 *
 * @note This macro is strictly active only in **Debug** builds. In **Release** builds
 *       (when `NDEBUG` is defined), it expands into a zero-cost void cast.
 * @note To ensure that the macro support compile-time evaluation an exception is thrown,
 *       instead of logging during compile time.
 *
 * @param condition The boolean expression to evaluate.
 * @param message   A string literal or streamable object providing context on the failure.
 */
#ifndef NDEBUG
    #define TEMPEST_ASSERT(condition, message)                                                                         \
        do                                                                                                             \
        {                                                                                                              \
            if (std::is_constant_evaluated())                                                                          \
            {                                                                                                          \
                if (!(condition))                                                                                      \
                    tempest::internal::compileTimeErrorTrap();                                                         \
            }                                                                                                          \
            else                                                                                                       \
            {                                                                                                          \
                if (!(condition))                                                                                      \
                {                                                                                                      \
                    tempest::internal::logAssertion(#condition, message, __FILE__, __LINE__);                          \
                    TEMPEST_DEBUG_BREAK();                                                                             \
                }                                                                                                      \
            }                                                                                                          \
        } while (false)
#else
    #define TEMPEST_ASSERT(condition, message) ((void) 0)
#endif