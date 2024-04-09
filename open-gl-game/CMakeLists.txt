# CMakeList.txt : CMake project for open-gl-game, include source and define
# project specific logic here.
#

find_package(GLEW REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

# Add source to this project's executable.
add_executable (open-gl-game "main.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET open-gl-game PROPERTY CXX_STANDARD 20)
  # CXX_STANDARD dictates which standard version the target will be compiled with. Specifically, at the time of writing the possible values are {98, 11, 14, 17, 20, 23, 26}.
	set_property(TARGET open-gl-game PROPERTY CXX_STANDARD 20)

	# CXX_STANDARD_REQUIRED is a boolean that makes the standard defined in CXX_STANDARD a requirement. Therefore, configuring the project will not succeed if the compiler used does not support the standard you specified if this flag is set to On.
	set_property(TARGET open-gl-game PROPERTY CXX_STANDARD_REQUIRED On)

	#CXX_EXTENSIONS is another boolean value. Briefly speaking, it tells CMake whether compiler extensions (such as inline assembly) are allowed or not. For more portable code, set this value to Off to ensure your code will compile in all compilers.
	set_property(TARGET open-gl-game PROPERTY CXX_EXTENSIONS Off)
endif()

target_link_libraries(open-gl-game 
	PUBLIC
	GLEW::GLEW
	SDL2::SDL2main
	imgui::imgui
)

# TODO: Add tests and install targets if needed.
