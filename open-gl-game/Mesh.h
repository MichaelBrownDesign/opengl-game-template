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

	class MeshData
	{
	public:
		std::optional<std::vector<glm::vec3>>	vertices;
		std::optional<std::vector<glm::vec3>>	normals;
		std::optional<std::vector<glm::vec4>>	colors;
		std::optional<std::vector<glm::vec2>>	uvs;
		std::optional<std::vector<GLuint>>		indices;
	};

	class Mesh
	{
	public:
		GLuint vao;
		GLuint vbo;
		GLuint ibo;
		GLuint vertexCount;
		GLuint indexCount;

		Mesh() :
			vao(0),
			vbo(0),
			vertexCount(0),
			indexCount(0)
		{}

		Mesh(GLuint vao, GLuint vbo, GLuint vertexCount) :
			vao(vao),
			vbo(vbo),
			vertexCount(vertexCount),
			indexCount(0)
		{}

		Mesh(GLuint vao, GLuint vbo, GLuint ibo, GLuint vertexCount, GLuint indexCount) :
			vao(vao),
			vbo(vbo),
			ibo(ibo),
			vertexCount(vertexCount),
			indexCount(indexCount)
		{}

		inline bool isValid() const { return vao != 0; }
		inline bool hasIndices() const { return ibo != 0; }
	};

	Mesh CreateMesh(const std::vector<Vertex>& vertices);
	Mesh CreateMesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices);
	void DeleteMesh(Mesh& mesh);
	void DrawMesh(const Mesh& mesh);
}