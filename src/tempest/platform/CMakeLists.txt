set(TEMPEST_PLATFORM_LIB TempestPlatform${TEMPEST_INTERNAL_TARGET_SUFFIX})
add_library(
        ${TEMPEST_PLATFORM_LIB}
        OBJECT
        sdl/ClockTime.cpp
        sdl/Memory.cpp
        sdl/Platform.cpp
)
# TODO: Add a platform based src file switching

target_sources(${TEMPEST_PLATFORM_LIB} PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${TEMPEST_ROOT_DIR}/platform
        FILES
        ClockTime.h
        Platform.h
        Memory.h
        PlatformDetect.h
)

add_library(Tempest::Internal::Platform ALIAS ${TEMPEST_PLATFORM_LIB})

target_compile_features(${TEMPEST_PLATFORM_LIB} PRIVATE cxx_std_23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

target_include_directories(${TEMPEST_PLATFORM_LIB} PUBLIC ${TEMPEST_ROOT_DIR})

target_link_libraries(
        ${TEMPEST_PLATFORM_LIB} PUBLIC # TODO: Update to private after init vulkan using pimpl idiom
        SDL3::SDL3-static
        Vulkan::Headers
)