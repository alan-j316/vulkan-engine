set(ENGINE_NAME Tempest)
add_library(
        ${ENGINE_NAME}
        Main.cpp
)
add_library(${ENGINE_NAME}::${ENGINE_NAME} ALIAS ${ENGINE_NAME})

target_compile_features(${ENGINE_NAME} PUBLIC cxx_std_23)
target_include_directories(${ENGINE_NAME} PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(
        ${ENGINE_NAME}
        PRIVATE
        ${PROJECT_NAME}-StrictWarnings
        SDL3::SDL3-static
        std::stb
)

set(SHADER_SOURCES basic.slang)
AddSlangShaderTarget(${ENGINE_NAME} SOURCES ${SHADER_SOURCES})

add_custom_command(
        TARGET ${ENGINE_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E
        copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/res/shaders"
        "$<TARGET_FILE_DIR:${ENGINE_NAME}>/shaders"
        COMMENT "Copying shaders to build directory..."
)

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