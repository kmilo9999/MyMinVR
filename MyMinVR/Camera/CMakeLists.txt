add_library( 
Camera
Camera.cpp
Camera.h
VRCamera.cpp
VRCamera.h
)

target_include_directories(Camera PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

install(FILES Camera.h   DESTINATION ${CMAKE_INSTALL_PREFIX}/Camera/include)
