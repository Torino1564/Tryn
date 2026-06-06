cmake_minimum_required(VERSION 3.20)

project(Tryn)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(CLI11 CONFIG REQUIRED)
find_path(TINYGLTF_INCLUDE_DIRS "tiny_gltf.h")

add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)

add_library(TrynGlobalSettings INTERFACE)

target_link_libraries(TrynGlobalSettings INTERFACE 
    CLI11::CLI11
    assimp::assimp
    imgui::imgui
    glm::glm
)

if(WIN32)
    target_link_libraries(TrynGlobalSettings INTERFACE
        d3d11
        dxgi
        dxguid
        d3dcompiler
        dwmapi
        user32
        gdi32
        shell32
        ole32
        comdlg32
        dbghelp

    )
endif()

target_include_directories(TrynGlobalSettings INTERFACE
    ${CMAKE_SOURCE_DIR}
    ${TINYGLTF_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}
)

target_compile_options(TrynGlobalSettings INTERFACE
    $<$<AND:$<CXX_COMPILER_ID:GNU>,$<PLATFORM_ID:Windows>>:-municode>
)

add_subdirectory(Core)
add_subdirectory(TrynGame)
