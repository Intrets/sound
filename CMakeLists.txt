cmake_minimum_required(VERSION 3.20)

include("./cmake/MakeModule.cmake")

set(MODULE_NAME sound)

project(${MODULE_NAME} LANGUAGES CXX)

make_module(
	MODULE_NAME ${MODULE_NAME}
	MODULE_FILES
		SoundPlayer
	CXX_STANDARD 20
	REQUIRED_LIBS
		SDL2::SDL2-static
		SDL_Mixer
		misc
		mem
		GLFW3
)

