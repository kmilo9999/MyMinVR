project(MeshLoader)

find_package(MinVR REQUIRED)	
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)


find_package(ASSIMP REQUIRED)
if(ASSIMP_FOUND)
    message(STATUS "Found ASSIMP in ${ASSIMP_INCLUDE_DIRS}")
    include_directories(${ASSIMP_INCLUDE_DIR})
else()
    message(STATUS "ASSIMP NOT Found ")
endif() 

find_package(ImguiVR REQUIRED)

if(ImguiVR_FOUND)
    message(STATUS "Found ImguiVR in ${IMGUIVR_INCLUDE_DIRS}")
	message(STATUS "Found ImguiVR in ${IMGUIVR_INCLUDE_DIR}")
	message(STATUS "Found ImguiVR in ${ImguiVR_INCLUDE_DIR}")
	message(STATUS "Found ImguiVR in ${ImguiVR_INCLUDE_DIRS}")
    include_directories(${IMGUIVR_INCLUDE_DIRS})
else()
    message(STATUS "ImguiVR NOT Found ")
endif() 

set(MINVR_PATH "${MINVR_INCLUDE_DIR}/MinVR-2.0/")

include_directories(
  common/
  #stb_image/
  ${OPENGL_INCLUDE_DIR}
  ${MINVR_INCLUDE_DIR}
  ${GLEW_INCLUDE_DIRS}
  ${GLM_INCLUDE_DIRS}
  ${ASSIMP_INCLUDE_DIRS}
  ${IMGUIVR_INCLUDE_DIRS} 
  ${IMGUIVR_INCLUDE_DIR} 
  ${ImguiVR_INCLUDE_DIR}  
  ${ImguiVR_INCLUDE_DIRS}
)



#set(PROJECT_SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR})

set(LIBRARIES 
  ${OPENGL_LIBRARY}
  ${GLEW_LIBRARY}
  ${GLM_LIBRARIES}
  ${IMGUIVR_LIBRARIES}

  stb_image
  AssimpLoader
  Camera
  GLMLoader
  Model
  ShaderProgram
  Terrain
  Texture
)

message( STATUS "message to display" ${LIBRARIES})  

# STB_IMAGE
#add_library(STB_IMAGE "stb_image/stb_image.cpp")

add_subdirectory(AssimpLoader)
add_subdirectory(Camera)
#add_subdirectory(common)
add_subdirectory(GLMLoader)
add_subdirectory(Model)
add_subdirectory(ShaderProgram)
add_subdirectory(stb_image)
add_subdirectory(Terrain)
add_subdirectory(Texture)


ADD_EXECUTABLE(MeshLoader 
    Main.cpp
	MyVRApp.cpp
	MyVRApp.h
	
)

target_link_libraries(MeshLoader
PUBLIC MinVR::MinVR
  ${LIBRARIES}
  
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

install(FILES MyVRApp.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include)

install(TARGETS MeshLoader EXPORT MeshLoader DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

target_include_directories(MeshLoader INTERFACE
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include> 
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include/meshLoader>)
	
install(FILES MeshLoader.cmake DESTINATION ${CMAKE_INSTALL_PREFIX}/)


