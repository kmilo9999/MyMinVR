add_library( 
Model
Model.cpp
Model.h
Transform.cpp
Transform.h
)

target_include_directories(Model PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")


install(FILES Model.h   DESTINATION ${CMAKE_INSTALL_PREFIX}/Model/include)
