cmake_minimum_required(VERSION 3.20)

file(GLOB_RECURSE CORE_SOURCES
    CONFIGURE_DEPENDS
    *.cpp
    *.h
    *.hpp
)

add_library(Core STATIC ${CORE_SOURCES})

target_include_directories(Core
PUBLIC
    include

PRIVATE
    src
    third
    third/stb_image
    third/uuid_v4
    third/imgui-node-editor-0.9.3
)

target_compile_definitions(Core PRIVATE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    CORE_STATIC
)

if(MSVC)

    target_compile_options(Core PRIVATE
        /W4
        /MP
        /EHsc
        /bigobj
        /utf-8
    )

endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
    ${TRYNCORE_SOURCES}
)

if(WIN32)

    target_link_libraries(Core PRIVATE
        d3d11
        dxgi
        dxguid
        d3dcompiler
    )

endif()

target_link_libraries(Core PRIVATE
    TrynGlobalSettings
)