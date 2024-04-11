#include "mesh.h"

namespace gfx
{
	// Stores vertices in a single VBO, Interleaved.
	// P = position
	// N = normal
	// U = uv
	// [P,N,U,P,N,U,P,N,U]
	void bufferVertices_Interleaved(const MeshData& meshData)
	{
		const auto vertexSize = meshData.vertexSize();
		const auto vertexCount = meshData.vertexCount();

		// Size of the vertex buffer
		const unsigned int bufferSize = vertexSize * vertexCount;

		char* vertexData = new char[bufferSize];

		int offset = 0;
		if (meshData.vertices.has_value())
		{
			const std::vector<glm::vec3>& vertices = meshData.vertices.value();
			for (int i = 0; i < vertices.size(); ++i)
			{
				memcpy(&vertexData[i * vertexSize + offset], &vertices[i], sizeof(glm::vec3));
			}

			// Position attribute pointer
			glEnableVertexAttribArray(0);
			// Point to a 3 component vector of type Float with an offset of {offset} bytes.
			glVertexAttribPointer(0, 3, GL_FLOAT, false, vertexSize, (const void*)offset);

			offset += sizeof(glm::vec3);
		}
		if (meshData.normals.has_value())
		{
			const std::vector<glm::vec3>& normals = meshData.normals.value();
			for (int i = 0; i < normals.size(); ++i)
			{
				memcpy(&vertexData[i * vertexSize + offset], &normals[i], sizeof(glm::vec3));
			}

			// Normals attribute pointer
			glEnableVertexAttribArray(1);
			// Point to a 3 component vector of type Float with an offset of {offset} bytes.
			glVertexAttribPointer(1, 3, GL_FLOAT, false, vertexSize, (const void*)offset);
			offset += sizeof(glm::vec3);
		}
		if (meshData.uvs.has_value())
		{
			const std::vector<glm::vec2>& uvs = meshData.uvs.value();
			for (int i = 0; i < uvs.size(); ++i)
			{
				memcpy(&vertexData[i * vertexSize + offset], &uvs[i], sizeof(glm::vec2));
			}

			// UVs attribute pointer
			glEnableVertexAttribArray(2);
			// Point to a 2 component vector of type Float with an offset of {offset} bytes.
			glVertexAttribPointer(2, 2, GL_FLOAT, false, vertexSize, (const void*)offset);
			offset += sizeof(glm::vec2);
		}
		if (meshData.colors.has_value())
		{
			const std::vector<glm::vec4>& colors = meshData.colors.value();
			for (int i = 0; i < colors.size(); ++i)
			{
				memcpy(&vertexData[i * vertexSize + offset], &colors[i], sizeof(glm::vec4));
			}

			// Colors attribute pointer
			glEnableVertexAttribArray(3);
			// Point to a 4 component vector of type Float with an offset of {offset} bytes.
			glVertexAttribPointer(3, 4, GL_FLOAT, false, vertexSize, (const void*)offset);
			offset += sizeof(glm::vec4);
		}

		// Write out vertex data to the buffer. Use STATIC_DRAW as we don't plan on updating the buffer.
		glBufferData(GL_ARRAY_BUFFER, bufferSize, vertexData, GL_STATIC_DRAW);

		delete[] vertexData;
	}

	// Stores vertices in a single VBO, Non-Interleaved.
	// P = position
	// N = normal
	// U = uv
	// [P,P,P,N,N,N,U,U,U]
	void bufferVertices_Seperate(const MeshData& meshData)
	{
		const auto vertexSize = meshData.vertexSize();
		const auto vertexCount = meshData.vertexCount();

		// Size of the vertex buffer
		const unsigned int bufferSize = vertexSize * vertexCount;

		char* vertexData = new char[bufferSize];

		int offset = 0;
		if (meshData.vertices.has_value())
		{
			memcpy(&vertexData[offset], &meshData.vertices.value()[0], sizeof(glm::vec3) * vertexCount);

			// Position attribute pointer
			glEnableVertexAttribArray(0);

			// Point to a 3 component vector of type Float with an offset of {offset} bytes.
			glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, (const void*)offset);

			offset += sizeof(glm::vec3) * vertexCount;
		}
		if (meshData.normals.has_value())
		{
			memcpy(&vertexData[offset], &meshData.normals.value()[0], sizeof(glm::vec3) * vertexCount);

			// Position attribute pointer
			glEnableVertexAttribArray(1);

			// Point to a 3 component vector of type Float with an offset of {offset} bytes.
			glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, (const void*)offset);

			offset += sizeof(glm::vec3) * vertexCount;
		}
		if (meshData.uvs.has_value())
		{
			memcpy(&vertexData[offset], &meshData.uvs.value()[0], sizeof(glm::vec2) * vertexCount);

			// Position attribute pointer
			glEnableVertexAttribArray(2);

			// Point to a 3 component vector of type Float with an offset of {offset} bytes.
			glVertexAttribPointer(2, 2, GL_FLOAT, false, 0, (const void*)offset);

			offset += sizeof(glm::vec2) * vertexCount;
		}
		if (meshData.colors.has_value())
		{
			memcpy(&vertexData[offset], &meshData.colors.value()[0], sizeof(glm::vec4) * vertexCount);

			// Position attribute pointer
			glEnableVertexAttribArray(3);

			// Point to a 3 component vector of type Float with an offset of {offset} bytes.
			glVertexAttribPointer(3, 4, GL_FLOAT, false, 0, (const void*)offset);

			offset += sizeof(glm::vec4) * vertexCount;
		}

		// Write out vertex data to the buffer. Use STATIC_DRAW as we don't plan on updating the buffer.
		glBufferData(GL_ARRAY_BUFFER, bufferSize, vertexData, GL_STATIC_DRAW);

		delete[] vertexData;
	}

	Mesh CreateMesh(const MeshData& meshData, bool interleaved)
	{
		// Check that vertices are > 0 in size and that all attributes match in length. (eg. same amount of vertex positions and normals).
		const auto vertexSize = meshData.vertexSize();
		const auto vertexCount = meshData.vertexCount();
		if (vertexSize == 0 || !meshData.validAttributeCount())
		{
			return Mesh();
		}

		GLuint vao = 0;
		GLuint vbo = 0;
		GLuint ibo = 0;

		// A Vertex Array Object (VAO) is an OpenGL Object that stores all of the state needed to supply vertex data.
		// It stores the format of the vertex data as well as the Buffer Objects providing the vertex data arrays.
		glGenVertexArrays(1, &vao);

		// Generate a buffer for the vertices
		glGenBuffers(1, &vbo);

		// Generate a buffer for the indices
		glGenBuffers(1, &ibo);

		// Bind the vao to capture our mesh attribtues
		glBindVertexArray(vao);

		// Bind the vertex buffer so we can modify it
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		if (interleaved)
		{
			bufferVertices_Interleaved(meshData);
		}
		else
		{
			bufferVertices_Seperate(meshData);
		}

		if (meshData.indices.has_value())
		{
			// Bind index buffer and upload index data
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * meshData.indices.value().size(), meshData.indices.value().data(), GL_STATIC_DRAW);
		}

		// Cleanup opengl state
		// Unbind VAO first so the IBO doesn't get removed from the VAO state vector.
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		if (meshData.indices.has_value())
		{
			return Mesh(vao, vbo, ibo, vertexCount, meshData.indices.value().size());
		}
		else
		{
			return Mesh(vao, vbo, vertexCount);
		}
	}

	void DeleteMesh(Mesh& mesh)
	{
		if (mesh.hasIndices())
		{
			glDeleteBuffers(1, &mesh.ibo);
		}

		glDeleteBuffers(1, &mesh.vbo);
		glDeleteVertexArrays(1, &mesh.vao);

		mesh.vertexCount = 0;
		mesh.indexCount = 0;
	}

	void DrawMesh(const Mesh& mesh)
	{
		glBindVertexArray(mesh.vao);
		if (mesh.hasIndices())
			glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, (void*)0);
		else
			glDrawArrays(GL_TRIANGLE_STRIP, 0, mesh.vertexCount);
		glBindVertexArray(0);
	}
}