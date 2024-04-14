#include <iostream>

#define NO_SDL_GLEXT
#include <GL/glew.h>
#include "SDL.h"
#include "SDL_opengl.h"
#undef main

#include "GLErrorCheck.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Mesh.h"
#include "Primitives.h"
#include "Camera.h"

using namespace std;

bool quit = false;
int windowWidth = 640;
int windowHeight = 480;
bool wireframe = false;

float nearPlane = 0.1f;
float farPlane = 100.0f;
float fieldOfView = 70;

gfx::ShaderHandle shader;
glm::mat4 projection;
glm::mat4 view;

Uint64 NOW = SDL_GetPerformanceCounter();
Uint64 LAST = 0;
double deltaTime = 0;

bool input_up;
bool input_down;
bool input_right;
bool input_left;
bool input_rightMouse;
glm::vec2 mouseDelta;
Camera camera(glm::vec3(0, 1, 3));

struct DrawCall
{
	gfx::Mesh mesh;
	glm::mat4 matrix;
};

std::vector<DrawCall> drawCalls;

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

void key_down(const SDL_KeyboardEvent& event)
{
	if (event.keysym.sym == SDLK_w) input_up = true;
	if (event.keysym.sym == SDLK_s) input_down = true;
	if (event.keysym.sym == SDLK_d) input_right = true;
	if (event.keysym.sym == SDLK_a) input_left = true;
	if (event.keysym.sym == SDLK_TAB) wireframe = !wireframe;
}
void key_up(const SDL_KeyboardEvent& event)
{
	if (event.keysym.sym == SDLK_w) input_up	= false;
	if (event.keysym.sym == SDLK_s) input_down	= false;
	if (event.keysym.sym == SDLK_d) input_right = false;
	if (event.keysym.sym == SDLK_a) input_left	= false;
}

void mouse_button_down(const SDL_MouseButtonEvent& event)
{
	if (event.button == SDL_BUTTON_RIGHT) input_rightMouse = true;
}
void mouse_button_up(const SDL_MouseButtonEvent& event)
{
	if (event.button == SDL_BUTTON_RIGHT) input_rightMouse = false;
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

		case SDL_KEYDOWN:
			key_down(event.key);
			break;
		case SDL_KEYUP:
			key_up(event.key);
			break;
		case SDL_MOUSEBUTTONDOWN:
			mouse_button_down(event.button);
			break;
		case SDL_MOUSEBUTTONUP:
			mouse_button_up(event.button);
			break;
		case SDL_MOUSEMOTION:
			mouseDelta.x = event.motion.xrel;
			mouseDelta.y = event.motion.yrel;
			break;
		}
	}
}

void begin_game()
{
	shader = gfx::CompileShader(gfx::default_lit_color);
	GL_ERRORCHECK();
	drawCalls =
	{
		{
			gfx::CreateMesh(gfx::primitive::Quad(1.0f, 1.0f), false),
			glm::translate(glm::identity<glm::mat4>(), glm::vec3(-1, 0, 0))
		},
		{
			gfx::CreateMesh(gfx::primitive::Box(1.0f, 1.0f, 1.0f), false),
			glm::translate(glm::identity<glm::mat4>(), glm::vec3(0, 0, 0))
		},
		{
			gfx::CreateMesh(gfx::primitive::Sphere(2, 0.5f)),
			glm::translate(glm::identity<glm::mat4>(), glm::vec3(0,0,0)),
		},
		{
			gfx::CreateMesh(gfx::primitive::Cylinder(0.5f, 1.0f, 16)),
			glm::translate(glm::identity<glm::mat4>(), glm::vec3(0,0,0)),
		},
		{
			gfx::CreateMesh(gfx::primitive::Capsule(0.5f, 1.0f, 16, 16, 0)),
			glm::translate(glm::identity<glm::mat4>(), glm::vec3(0,0,0)),
		},
	};

	projection = glm::perspective(	glm::radians(fieldOfView),					// The vertical Field of View in radians (the amount of "zoom").
									(float) windowWidth / (float) windowHeight,	// Aspect Ratio.
									nearPlane,									// Near clipping plane. Keep as big as possible, or you'll get precision issues.
									farPlane);									// Far clipping plane. Keep as little as possible.

	//view = glm::translate(glm::identity<glm::mat4>(), glm::vec3(0, 0, -5));
	glm::vec3 cameraPosition = glm::vec3(0, 1, -3);
	view = glm::translate(glm::identity<glm::mat4>(), cameraPosition) *
			glm::lookAt(cameraPosition, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	auto attribs = gfx::GetShaderVertexAttributes(shader);

	for (const auto& attrib : attribs)
	{
		std::cout << attrib.name << ": size=" << attrib.size << ", type=" << attrib.type << ", location=" << attrib.location << std::endl;
	}
}

void end_game()
{
	gfx::DeleteShader(shader);
	for (auto& drawcall : drawCalls)
	{
		gfx::DeleteMesh(drawcall.mesh);
	}
}

void game_loop(SDL_Window* window)
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	begin_game();

	GL_ERRORCHECK();

	GLint mvpLocation = gfx::GetShaderUniformLocation(shader, "mvp");
	GLint modelLocation = gfx::GetShaderUniformLocation(shader, "model");
	GLint colorLocation = gfx::GetShaderUniformLocation(shader, "color");
	GLint lightDirLocation = gfx::GetShaderUniformLocation(shader, "lightDir");

	GL_ERRORCHECK();

	while (!quit)
	{
		LAST = NOW;
		NOW = SDL_GetPerformanceCounter();
		// Calculate the Milliseconds since last frame
		deltaTime = ((NOW - LAST) * 1000 / (double)SDL_GetPerformanceFrequency());
		// Convert to seconds
		deltaTime *= 0.001;

		process_events();
		if (input_up)		camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
		if (input_down)		camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
		if (input_right)	camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
		if (input_left)		camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
		if (input_rightMouse)
		{
			camera.ProcessMouseMovement(mouseDelta.x, -mouseDelta.y);
		}
		mouseDelta = glm::vec2(0, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (wireframe)      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		gfx::UseShader(shader);

		int index = 0;
		for (auto& drawcall : drawCalls)
		{
			drawcall.matrix = glm::translate(glm::identity<glm::mat4>(), glm::vec3(-3 + index++ * 2, 0, 0));
								//* glm::rotate(glm::identity<glm::mat4>(), (float)((SDL_GetTicks() / 100) % 360), glm::vec3(0.f, 1.f, 0.f));
				
			glm::mat4 mvp = projection * camera.GetViewMatrix() * drawcall.matrix;
			glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, &mvp[0][0]);
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &drawcall.matrix[0][0]);
			glm::vec4 color(0.f, 1.f, 0.f, 1.f);
			glUniform4fv(colorLocation, 1, &color[0]);
			glm::vec3 lightDir = glm::normalize(glm::vec3(-1.5, 2, 1));
			glUniform3fv(lightDirLocation, 1, &lightDir[0]);
			gfx::DrawMesh(drawcall.mesh);
		}

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
