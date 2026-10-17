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