set(TEMPEST_RESOURCE_LIB TempestResource${TEMPEST_INTERNAL_TARGET_SUFFIX})
add_library(
        ${TEMPEST_RESOURCE_LIB}
        STATIC
        DummyRemove.cpp
)

target_sources(${TEMPEST_RESOURCE_LIB} PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${TEMPEST_ROOT_DIR}/resources
        FILES
        ResourceTypes.h
)

add_library(Tempest::Internal::Resources ALIAS ${TEMPEST_RESOURCE_LIB})

target_compile_features(${TEMPEST_RESOURCE_LIB} PRIVATE cxx_std_23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

target_include_directories(${TEMPEST_RESOURCE_LIB} PUBLIC ${TEMPEST_ROOT_DIR})

#target_link_libraries(
#        ${TEMPEST_RESOURCE_LIB} PUBLIC
#        Tempest::Internal::Renderer::Vulkan
#)