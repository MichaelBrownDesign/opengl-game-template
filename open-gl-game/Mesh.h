#pragma once
#include <optional>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

namespace gfx
{
	struct Vertex
	{
		glm::vec3 position;
		glm::vec2 uv;
	};

	class Mesh
	{
	public:
		GLuint vao;
		GLuint vbo;
		GLuint size;

		Mesh() :
			vao(0),
			vbo(0),
			size(0)
		{}

		Mesh(GLuint vao, GLuint vbo, GLuint size) :
			vao(vao),
			vbo(vbo),
			size(size)
		{}

		inline bool isValid() const { return vao != 0 && vbo != 0; }
	};

	Mesh CreateMesh(const std::vector<Vertex>& vertices);
	void DeleteMesh(Mesh& mesh);
	void DrawMesh(const Mesh& mesh);
}