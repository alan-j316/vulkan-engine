set(ENGINE_NAME Tempest)
set(TEMPEST_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(TEMPEST_INTERNAL_TARGET_SUFFIX _Internal)
add_library(
        ${ENGINE_NAME}
        $<TARGET_OBJECTS:Tempest::Internal::Core>
        $<TARGET_OBJECTS:Tempest::Internal::Platform>
        containers/PMRResource.h
        containers/Vector.h
)
add_library(${ENGINE_NAME}::${ENGINE_NAME} ALIAS ${ENGINE_NAME})

# TODO: Add dynamic linked library
# TODO: Supply TEMPEST_LIB_BUILD & TEMPEST_DLL compile definitions as necessary (Enable Position Independent Code)


target_sources(
        ${ENGINE_NAME} PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        FILES
        Tempest.h
        Entry.h
        Game.h
)
target_compile_features(${ENGINE_NAME} PUBLIC cxx_std_23)
target_include_directories(${ENGINE_NAME} PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

add_subdirectory(core)
add_subdirectory(platform)
add_subdirectory(renderer)

target_link_libraries(
        ${ENGINE_NAME}
        PUBLIC
        ${PROJECT_NAME}-StrictWarnings
        Tempest::Internal::Core
        Tempest::Internal::Platform
        Tempest::Internal::Renderer
        SDL3::SDL3-static
        std::stb
)

set(SHADER_SOURCES basic.slang)
AddSlangShaderTarget(${ENGINE_NAME} SOURCES ${SHADER_SOURCES})



#add_custom_command(
#        TARGET ${ENGINE_NAME}
#        POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E
#        copy_directory
#        "${CMAKE_CURRENT_SOURCE_DIR}/res/textures"
#        "$<TARGET_FILE_DIR:${ENGINE_NAME}>/textures"
#        COMMENT "Copying textures to build directory..."
#)
#
#add_custom_command(
#        TARGET ${ENGINE_NAME}
#        POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E
#        copy_directory
#        "${CMAKE_CURRENT_SOURCE_DIR}/res/models"
#        "$<TARGET_FILE_DIR:${ENGINE_NAME}>/models"
#        COMMENT "Copying models to build directory..."
#)