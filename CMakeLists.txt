cmake_minimum_required(VERSION 3.29)
project(upscaler VERSION 1.0.0)

find_package(OpenGL REQUIRED)

set(CMAKE_CXX_STANDARD 23)

add_executable(upscaler
        src/main.cpp
        src/glad.c
        src/triangle_mesh.cpp
)

target_include_directories(upscaler PRIVATE dependencies/include)

target_link_libraries(upscaler PRIVATE "${CMAKE_SOURCE_DIR}/dependencies/lib/libglfw3.a" OpenGL::GL)
if(WIN32)
    target_link_libraries(upscaler PRIVATE opengl32)
endif()
