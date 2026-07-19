cmake_minimum_required(VERSION 3.20)
project(RendererEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(RENDERER_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(LIBRARY_ROOT ${RENDERER_ROOT}/Libraries)
set(GLFW_ROOT ${LIBRARY_ROOT}/glfw-3.4.bin.WIN64)
set(GLEW_ROOT ${LIBRARY_ROOT}/glew-2.3.1)
set(IMGUI_ROOT ${LIBRARY_ROOT}/imgui)

find_package(OpenGL REQUIRED)

add_library(RendererEngine STATIC
    Core/Application.cpp
    ThirdParty/stb_image.cpp
    Core/ConsoleLog.cpp
    Editor/EditorController.cpp
    Editor/EditorGui.cpp
    Input/Input.cpp
    Platform/Window.cpp
    Renderer/Camera.cpp
    Renderer/Framebuffer.cpp
    Renderer/Mesh.cpp
    Renderer/ModelLoader.cpp
    Renderer/Renderer.cpp
    Renderer/Shader.cpp
    Renderer/Texture.cpp
    Renderer/VertexArray.cpp
    Renderer/VertexBuffer.cpp
    Renderer/IndexBuffer.cpp
    Scene/Scene.cpp
    Scene/Transform.cpp
    ${IMGUI_ROOT}/imgui.cpp
    ${IMGUI_ROOT}/imgui_demo.cpp
    ${IMGUI_ROOT}/imgui_draw.cpp
    ${IMGUI_ROOT}/imgui_tables.cpp
    ${IMGUI_ROOT}/imgui_widgets.cpp
    ${IMGUI_ROOT}/backends/imgui_impl_glfw.cpp
    ${IMGUI_ROOT}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(RendererEngine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GLFW_ROOT}/include
    ${GLEW_ROOT}/include
    ${IMGUI_ROOT}
    ${IMGUI_ROOT}/backends
)

if(MINGW)
    set(GLFW_LIBRARY ${GLFW_ROOT}/lib-mingw-w64/libglfw3.a)

    find_library(GLEW_LIBRARY
        NAMES glew32 libglew32.dll.a
        PATHS
            C:/msys64/ucrt64/lib
            C:/msys64/mingw64/lib
        NO_DEFAULT_PATH
    )

    if(NOT GLEW_LIBRARY)
        message(FATAL_ERROR "GLEW for MinGW was not found. Install it with: pacman -S mingw-w64-ucrt-x86_64-glew")
    endif()

elseif(MSVC)
    set(GLFW_LIBRARY ${GLFW_ROOT}/lib-vc2022/glfw3.lib)
    set(GLEW_LIBRARY ${GLEW_ROOT}/lib/Release/x64/glew32.lib)

else()
    message(FATAL_ERROR "Unsupported compiler. Use MSYS2 MinGW/UCRT64 or Visual Studio.")
endif()

target_link_libraries(RendererEngine PUBLIC
    OpenGL::GL
    ${GLFW_LIBRARY}
    ${GLEW_LIBRARY}
)

# Main executable
add_executable(Merso
    Sandbox/Main.cpp
    Resources/app.rc
)

target_link_libraries(Merso PRIVATE RendererEngine)

# Copy shaders after build
add_custom_command(TARGET Merso POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/Shaders
        $<TARGET_FILE_DIR:Merso>/Shaders
)

# Copy DLLs when using MSVC
if(MSVC)
    add_custom_command(TARGET Merso POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${GLEW_ROOT}/bin/Release/x64/glew32.dll
            $<TARGET_FILE_DIR:Merso>/glew32.dll
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${GLFW_ROOT}/lib-vc2022/glfw3.dll
            $<TARGET_FILE_DIR:Merso>/glfw3.dll
    )
endif()