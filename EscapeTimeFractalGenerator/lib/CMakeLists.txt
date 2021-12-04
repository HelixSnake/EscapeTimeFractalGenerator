cmake_minimum_required(VERSION 3.20)

#SOIL
ADD_LIBRARY(soil STATIC
  SOIL/src/image_helper.c
  SOIL/src/stb_image_aug.c
  SOIL/src/image_DXT.c
  SOIL/src/SOIL.c
  )

target_include_directories(soil PUBLIC SOIL/inc)

#IMGUI
ADD_LIBRARY(imgui STATIC
  imgui/imgui.cpp
  imgui/imgui_demo.cpp
  imgui/imgui_draw.cpp
  imgui/imgui_tables.cpp
  imgui/imgui_widgets.cpp
  imgui/backends/imgui_impl_glfw.cpp
  imgui/backends/imgui_impl_opengl3.cpp
  )

#we use the glfw backend, introducing that dependency
target_include_directories(imgui PUBLIC imgui)
target_include_directories(imgui PUBLIC glfw/include)

#GLFW
add_subdirectory(glfw)

#GLM
add_subdirectory(glm)

#GLEW
add_compile_definitions(GLEW_STATIC)
ADD_LIBRARY(glew STATIC glew/src/glew.c)
target_include_directories(glew PUBLIC glew/include)