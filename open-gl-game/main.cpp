#include <iostream>

#define NO_SDL_GLEXT
#include <GL/glew.h>
#include "SDL.h"
#include "SDL_opengl.h"
#undef main

#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "mesh.h"

using namespace std;

bool quit = false;
int windowWidth = 640;
int windowHeight = 480;

float nearPlane = 0.1f;
float farPlane = 100.0f;
float fieldOfView = 90;

gfx::ShaderHandle shader;
gfx::Mesh mesh;
glm::mat4 projection;
glm::mat4 view;
glm::mat4 model;

int startup(SDL_Window*& window, const char* title, int width, int height)
{
	int initError = SDL_Init(SDL_INIT_VIDEO);

	if (int errorMajor = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3))
	{
		return errorMajor;
	}

	if (int errorMinor = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3))
	{
		return errorMinor;
	}

	window = SDL_CreateWindow(title,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		width,
		height,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	if (window == nullptr)
	{
		cerr << "Failed to create SDL window" << endl;
		return 1;
	}

	SDL_GLContext ctx = SDL_GL_CreateContext(window);

	if (ctx == nullptr)
	{
		SDL_DestroyWindow(window);
		cerr << "Failed to create SDL Context: " << SDL_GetError() << endl;
		return 2;
	}

	if (int makeCurrentError = SDL_GL_MakeCurrent(window, ctx))
	{
		SDL_DestroyWindow(window);
		cerr << "Failed to make SDL Context current" << endl;
		return 3;
	}

	
	glewExperimental = GL_TRUE;
	if (auto glewInitCode = glewInit() != GLEW_OK)
	{
		auto errorMsg = glewGetErrorString(glewInitCode);
		cerr << "Failed to initialize GLEW: " << errorMsg << endl;
		return glewInitCode;
	}

	return 0;
}

void shutdown(SDL_Window*& window)
{
	// Shutdown and cleanup
	if (window != nullptr)
	{
		SDL_DestroyWindow(window);
		window = nullptr;
	}
	SDL_Quit();
}

void process_events()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			quit = true;
			break;
		case SDL_WINDOWEVENT:
			switch (event.window.event)
			{
			case SDL_WINDOWEVENT_RESIZED:
				windowWidth = event.window.data1;
				windowHeight = event.window.data2;
				glViewport(0, 0, windowWidth, windowHeight);
				break;
			}
			break;
		}
	}
}

void begin_game()
{
	shader = gfx::CompileShader(gfx::default_shader_source);

	std::vector<gfx::Vertex> vertices(4);
	vertices[0].position = glm::vec3(0, 0, 0);
	vertices[1].position = glm::vec3(0, 1, 0);
	vertices[2].position = glm::vec3(1, 1, 0);
	vertices[3].position = glm::vec3(1, 0, 0);

	mesh = gfx::CreateMesh(vertices);

	projection = glm::perspective(	glm::radians(fieldOfView),					// The vertical Field of View, in radians: the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
									(float) windowWidth / (float) windowHeight,	// Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960, sounds familiar?
									nearPlane,									// Near clipping plane. Keep as big as possible, or you'll get precision issues.
									farPlane);									// Far clipping plane. Keep as little as possible.

	view = glm::identity<glm::mat4>();
	model = glm::identity<glm::mat4>();
	auto attribs = gfx::GetShaderVertexAttributes(shader);

	for (const auto& attrib : attribs)
	{
		std::cout << attrib.name << ": size=" << attrib.size << ", type=" << attrib.type << ", location=" << attrib.location << std::endl;
	}
}

void end_game()
{
	gfx::DeleteShader(shader);
	gfx::DeleteMesh(mesh);
}

void game_loop(SDL_Window* window)
{
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

	begin_game();

	while (!quit)
	{
		process_events();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		gfx::UseShader(shader);
		glm::mat4 mvp = projection * view * model;
		GLint location = gfx::GetShaderUniformLocation(shader, "mvp");
		glUniformMatrix4fv(location, 1, GL_FALSE, &mvp[0][0]);
		gfx::DrawMesh(mesh);

		SDL_GL_SwapWindow(window);
	}

	end_game();
}


int main()
{
	SDL_Window* window;

	if (startup(window, "OpenGL-Game", 640, 480))
	{
		return 1;
	}

	game_loop(window);

	shutdown(window);

	return 0;
}
