add_library( 
AssimpLoader
AssimpLoader.cpp
AssimpLoader.h
)


target_include_directories(AssimpLoader PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

install(FILES AssimpLoader.h   DESTINATION ${CMAKE_INSTALL_PREFIX}/AssimpLoader/include)
