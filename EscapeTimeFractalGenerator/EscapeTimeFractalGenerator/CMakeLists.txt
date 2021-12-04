add_executable(Escape 
  main.cpp
  ComplexFloat.cpp
  ComplexFractal.cpp
  EscapeTimeFractal.cpp
  FractalDictionary.cpp
  FractalDrawer.cpp
  FractalInterpreter.cpp
  FractalSmoothZoomer.cpp
  MandelbrotFractal.cpp
  QuadDrawer.cpp) 

target_link_libraries(Escape
  soil
  imgui
  glfw
  glm
  glew
  opengl32)

set(LIB "${PROJECT_SOURCE_DIR}/lib/")
target_include_directories(Escape PUBLIC
  "${LIB}glew/include"
  "${LIB}glfw/include"
  "${LIB}glm/glm"
  "${LIB}imgui"
  "${LIB}imgui/backends"
  "${LIB}imgui-filebrowser"
  "${LIB}SOIL/inc/SOIL")

install(TARGETS Escape
  DESTINATION bin)
install(FILES
  colorRamp.png
  imgui.ini
  DESTINATION bin)