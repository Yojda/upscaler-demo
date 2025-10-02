cmake_minimum_required(VERSION 3.29)
project(upscaler VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)

# Source files
set(SOURCES
        src/main.cpp
        src/glad.c
        src/triangle_mesh.cpp
        src/Shader.cpp
        dependencies/include/stb_image/stb_image.cpp
        dependencies/include/imgui/imgui.cpp
        dependencies/include/imgui/imgui_draw.cpp
        dependencies/include/imgui/imgui_widgets.cpp
        dependencies/include/imgui/imgui_tables.cpp
        dependencies/include/imgui/backends/imgui_impl_glfw.cpp
        dependencies/include/imgui/backends/imgui_impl_opengl3.cpp
)

add_executable(upscaler ${SOURCES})

target_include_directories(upscaler PRIVATE dependencies/include dependencies/include/imgui)

target_link_libraries(upscaler PRIVATE "${CMAKE_SOURCE_DIR}/dependencies/lib/libglfw3.a" OpenGL::GL)
if(WIN32)
    target_link_libraries(upscaler PRIVATE opengl32)
endif()
