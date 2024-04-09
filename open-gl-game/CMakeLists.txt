# CMakeList.txt : CMake project for open-gl-game, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (open-gl-game "main.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET open-gl-game PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
