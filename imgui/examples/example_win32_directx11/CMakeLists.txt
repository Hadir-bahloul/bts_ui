cmake_minimum_required(VERSION 3.10)
project(ImGuiExample)

set(CMAKE_CXX_STANDARD 17)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Add GLFW
add_subdirectory(external/glfw)  # Assume GLFW is in "external/glfw"

# Include ImGui
include_directories(imgui)
include_directories(imgui/backends)

# Create an ImGui source group
set(IMGUI_SOURCES
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_opengl3.cpp
)

add_executable(ImGuiExample main.cpp ${IMGUI_SOURCES})

# Link libraries
target_link_libraries(ImGuiExample glfw OpenGL::GL)
