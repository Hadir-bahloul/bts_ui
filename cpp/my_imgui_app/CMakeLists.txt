cmake_minimum_required(VERSION 3.10)
project(MyDearImGuiApp)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Path to the extracted GLFW folder (use forward slashes)
set(GLFW_DIR "G:/acer/Desktop/PRIMATEC_PFE/cpp/my_imgui_app/glfw-3.4.bin.WIN64")




# Add GLFW include directory
include_directories(${GLFW_DIR}/include)

# Add GLFW library directory (choose the appropriate library for your compiler)
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # For Visual Studio (MSVC)
    link_directories(${GLFW_DIR}/lib-vc2022)
    set(GLFW_LIBRARY ${GLFW_DIR}/lib-vc2022/glfw3.lib)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # For MinGW
    link_directories(${GLFW_DIR}/lib-mingw-w64)
    set(GLFW_LIBRARY ${GLFW_DIR}/lib-mingw-w64/libglfw3.a)
endif()

# Find OpenGL
find_package(OpenGL REQUIRED)

# Add Dear ImGui source files
set(IMGUI_SOURCES
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_opengl3.cpp
)

# Add Dear ImGui include directories
include_directories(imgui imgui/backends)

# Add your application source file
add_executable(MyDearImGuiApp main.cpp ${IMGUI_SOURCES})

# Link libraries
target_link_libraries(MyDearImGuiApp ${GLFW_LIBRARY} ${OPENGL_LIBRARIES})