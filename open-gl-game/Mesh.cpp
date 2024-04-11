#include "mesh.h"

namespace gfx
{
	Mesh CreateMesh(const std::vector<Vertex>& vertices)
	{
		GLuint vao;
		GLuint vbo;

		// A Vertex Array Object (VAO) is an OpenGL Object that stores all of the state needed to supply vertex data.
		// It stores the format of the vertex data as well as the Buffer Objects providing the vertex data arrays.
		glGenVertexArrays(1, &vao);

		// Generate a buffer for the vertices
		glGenBuffers(1, &vbo);

		// Bind the vao to capture our mesh attribtues
		glBindVertexArray(vao);

		// Size of the vertex buffer
		const unsigned int bufferSize = sizeof(Vertex) * vertices.size();

		// Bind the vertex buffer so we can modify it
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		// Write out vertex data to the buffer. Use STATIC_DRAW as we don't plan on updating the buffer.
		glBufferData(GL_ARRAY_BUFFER, bufferSize, vertices.data(), GL_STATIC_DRAW);


		// Vertex attribute pointer
		glEnableVertexAttribArray(0);
		// Point to a 3 component vector of type Float with an offset of 0 bytes.
		glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), (const void*)0);

		// UV attribute pointer
		glEnableVertexAttribArray(1);
		// Point to a 2 component vector of type Float with an offset of 12 bytes.
		glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(Vertex), (const void*)(sizeof(float) * 3));

		// Cleanup opengl state
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		if (vao)
		{
			return Mesh(vao, vbo, (GLuint)vertices.size());
		}
		else
		{
			return Mesh();
		}
	}

	Mesh CreateMesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices)
	{
		GLuint vao;
		GLuint vbo;
		GLuint ibo;

		// A Vertex Array Object (VAO) is an OpenGL Object that stores all of the state needed to supply vertex data.
		// It stores the format of the vertex data as well as the Buffer Objects providing the vertex data arrays.
		glGenVertexArrays(1, &vao);

		// Generate a buffer for the vertices
		glGenBuffers(1, &vbo);
		// Generate a buffer for the indices
		glGenBuffers(1, &ibo);

		// Bind the vao to capture our mesh attribtues
		glBindVertexArray(vao);

		// Size of the vertex buffer
		const unsigned int bufferSize = sizeof(Vertex) * vertices.size();

		// Bind the vertex buffer so we can modify it
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		// Write out vertex data to the buffer. Use STATIC_DRAW as we don't plan on updating the buffer.
		glBufferData(GL_ARRAY_BUFFER, bufferSize, vertices.data(), GL_STATIC_DRAW);


		// Vertex attribute pointer
		glEnableVertexAttribArray(0);
		// Point to a 3 component vector of type Float with an offset of 0 bytes.
		glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), (const void*)0);

		// UV attribute pointer
		glEnableVertexAttribArray(1);
		// Point to a 2 component vector of type Float with an offset of 12 bytes.
		glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(Vertex), (const void*)(sizeof(float) * 3));

		// Bind indices buffer and upload data
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

		// Cleanup opengl state
		// Unbind VAO first so the IBO doesn't get removed from the VAO state vector.
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		if (vao)
		{
			return Mesh(vao, vbo, ibo, (GLuint)vertices.size(), (GLuint)indices.size());
		}
		else
		{
			return Mesh();
		}
	}

	void DeleteMesh(Mesh& mesh)
	{
		glDeleteBuffers(1, &mesh.vbo);
		glDeleteVertexArrays(1, &mesh.vao);
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