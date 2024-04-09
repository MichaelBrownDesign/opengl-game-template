#include <iostream>

#define NO_SDL_GLEXT
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#undef main

using namespace std;

int main()
{
	cout << "Hello OpenGL!" << endl;

	return 0;
}
