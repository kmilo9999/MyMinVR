add_library( 
Terrain
Terrain.cpp
Terrain.h

)

target_link_libraries(Terrain PUBLIC stb_image)


target_include_directories(Terrain PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

install(FILES Terrain.h   DESTINATION ${CMAKE_INSTALL_PREFIX}/Terrain/include)
