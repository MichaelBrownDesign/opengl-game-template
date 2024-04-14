#pragma once
#include <optional>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

namespace gfx
{
	class MeshData
	{
	public:
		std::optional<std::vector<glm::vec3>>	vertices;
		std::optional<std::vector<glm::vec3>>	normals;
		std::optional<std::vector<glm::vec2>>	uvs;
		std::optional<std::vector<glm::vec4>>	colors;
		std::optional<std::vector<GLuint>>		indices;

		inline unsigned int vertexSize() const
		{
			unsigned int size = 0;
			if (vertices.has_value())	size += sizeof(glm::vec3);
			if (normals.has_value())	size += sizeof(glm::vec3);
			if (colors.has_value())		size += sizeof(glm::vec4);
			if (uvs.has_value())		size += sizeof(glm::vec2);
			return size;
		}

		inline size_t vertexCount() const
		{
			const size_t vertexCount = vertices.has_value() ? vertices.value().size() : 0;
			const size_t normalsCount = normals.has_value() ? normals.value().size() : 0;
			const size_t colorsCount = colors.has_value() ? colors.value().size() : 0;
			const size_t uvCount = uvs.has_value() ? uvs.value().size() : 0;
			return std::max({ vertexCount, normalsCount, colorsCount, uvCount });
		}

		inline bool validAttributeCount() const
		{
			const size_t vertexCount	= vertices.has_value() ? vertices.value().size() : 0;
			const size_t normalsCount	= normals.has_value() ? normals.value().size() : 0;
			const size_t colorsCount	= colors.has_value() ? colors.value().size() : 0;
			const size_t uvCount		= uvs.has_value() ? uvs.value().size() : 0;
			const size_t max			= std::max({ vertexCount, normalsCount, colorsCount, uvCount });
			return	(vertexCount == 0 || vertexCount == max) &&
					(normalsCount == 0 || normalsCount == max) &&
					(colorsCount == 0 || colorsCount == max) &&
					(uvCount == 0 || uvCount == max);
		}
	};

	class Mesh
	{
	public:
		GLuint vao;
		GLuint vbo;
		GLuint ibo;
		size_t vertexCount;
		size_t indexCount;

		Mesh() :
			vao(0),
			vbo(0),
			vertexCount(0),
			indexCount(0)
		{}

		Mesh(GLuint vao, GLuint vbo, size_t vertexCount) :
			vao(vao),
			vbo(vbo),
			vertexCount(vertexCount),
			indexCount(0)
		{}

		Mesh(GLuint vao, GLuint vbo, GLuint ibo, size_t vertexCount, size_t indexCount) :
			vao(vao),
			vbo(vbo),
			ibo(ibo),
			vertexCount(vertexCount),
			indexCount(indexCount)
		{}

		inline bool isValid() const { return vao != 0; }
		inline bool hasIndices() const { return ibo != 0; }
	};

	Mesh CreateMesh(const MeshData& meshData, bool interleaved = true);
	void DeleteMesh(Mesh& mesh);
	void DrawMesh(const Mesh& mesh);
}