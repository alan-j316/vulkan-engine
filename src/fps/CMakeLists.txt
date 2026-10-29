set(FPS_NAME TempestFPS)
add_executable(${FPS_NAME}
        Entry.cpp
        FPSGame.cpp
)

target_sources(${FPS_NAME} PRIVATE
        FPSGame.h
)

target_compile_features(${FPS_NAME} PUBLIC cxx_std_23)
target_include_directories(${FPS_NAME} PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(
        ${FPS_NAME}
        PRIVATE
        ${PROJECT_NAME}-StrictWarnings
        Tempest::Tempest
)


add_custom_command(
        TARGET ${FPS_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E
        copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${FPS_NAME}>/assets"
        COMMENT "Copying assets to build directory..."
)

#function(AddSlangShader TARGET)
#    cmake_parse_arguments("SHADER" "" "" "SOURCES" ${ARGN})
#    set(SHADERS_DIR "<TARGET_FILE_DIR:${FPS_NAME}>/shaders")
#    set(ENTRY_POINTS -entry vertMain -entry fragMain)
#    add_custom_command(
#            OUTPUT ${SHADERS_DIR}
#            COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADERS_DIR}
#    )
#    add_custom_command(
#            OUTPUT ${SHADERS_DIR}/Simple.frag.spv
#            COMMAND ${SLANGC_EXECUTABLE} ${SHADER_SOURCES} -target spirv -profile spirv_1_4 -emit-spirv-directly -fvk-use-entrypoint-name ${ENTRY_POINTS} -o Simple.frag.spv
#            WORKING_DIRECTORY ${SHADERS_DIR}
#            DEPENDS ${SHADERS_DIR} ${SHADER_SOURCES}
#            COMMENT "Compiling Slang Shaders"
#            VERBATIM
#    )
#
#endfunction()