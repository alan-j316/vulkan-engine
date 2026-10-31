set(TEMPEST_CORE_LIB TempestCore${TEMPEST_INTERNAL_TARGET_SUFFIX})
add_library(
        ${TEMPEST_CORE_LIB}
        OBJECT
        Logger.cpp
        Application.cpp
        MemorySubsystem.cpp
        Event.cpp
        Input.cpp
        EventListeners.cpp
        Clock.cpp
        AssetManager.cpp
)
add_library(Tempest::Internal::Core ALIAS ${TEMPEST_CORE_LIB})


target_sources(
        ${TEMPEST_CORE_LIB} PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${TEMPEST_ROOT_DIR}/core
        FILES
        Api.h
        AppConfig.h
        Application.h
        Asserts.h
        Logger.h
        MemorySubsystem.h
        Event.h
        Input.h
        Clock.h
        FileReader.h
        EventListeners.h
        AssetManager.h
        Vertex.h
)

target_include_directories(${TEMPEST_CORE_LIB} PUBLIC ${TEMPEST_ROOT_DIR})

target_compile_features(${TEMPEST_CORE_LIB} PRIVATE cxx_std_23)
set_target_properties(${TEMPEST_CORE_LIB} PROPERTIES CXX_STANDARD_REQUIRED ON)

target_link_libraries(
        ${TEMPEST_CORE_LIB} PRIVATE
        quill::quill
        fastgltf::fastgltf
)